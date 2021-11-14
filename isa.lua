-- ISA project - a protocol dissector
-- Peter Urgoš (xurgos00)
-- 11/2021

-- Used Resources:
	-- https://gitlab.com/wireshark/wireshark/-/wikis/Lua/Dissectors
	-- https://mika-s.github.io/wireshark/lua/dissector/2017/11/04/creating-a-wireshark-dissector-in-lua-1.html
	-- https://www.wireshark.org/docs/wsdg_html_chunked/wslua_dissector_example.html
	-- https://www.wireshark.org/docs/wsdg_html_chunked/wsluarm_modules.html


isa_proto = Proto("ISA", "ISA Protocol")


function isa_proto.dissector(buffer, pinfo, tree)
	local buffer_len = buffer:len()

	if buffer_len == 0 then return 0 end

	-- Set column name
	pinfo.cols.protocol = isa_proto.name

	-- Add root node of this protocol to the tree
	local subtree = tree:add(isa_proto, buffer(), "ISA Protocol Data")

	subtree:add(buffer(), "Message Length: " .. buffer_len)
	subtree:add(buffer(), "Message Raw: " .. buffer():string())

	-- --------------- --
	-- Server response --
	-- --------------- --
	if (pinfo.src_port == 32323) then
		subtree:add("Sender: Server")

		-- -- --
		-- OK --
		-- -- --
		if buffer(1, 2):string() == "ok" then
			subtree:add(buffer(1, 2), "Response Status: Ok")

			-- List command response
			if buffer(buffer_len - 3, 2):string() == "))" or buffer(buffer_len - 3, 2):string() == "()" then
				msgs_subtree = subtree:add(buffer(4, buffer_len - 5), "Dummy text")

				local msg_count = 0

				local temp_start = 5

				while buffer(temp_start, 1):string() == "(" do
					msg_subtree = msgs_subtree:add(msg_count)

					-- ID
					local temp_len = firstIndexOf(buffer(temp_start + 1):string(), " ") - 1
					msg_subtree:add(buffer(temp_start + 1, temp_len), "Id: " .. buffer(temp_start + 1, temp_len):string())

					-- Sender
					temp_start = temp_start + temp_len + 2
					temp_len = getSecondQuotePosition(buffer(temp_start):string())
					msg_subtree:add(buffer(temp_start, temp_len), "Sender: " .. buffer(temp_start + 1, temp_len - 2):string())

					-- Subject
					temp_start = temp_start + temp_len + 1
					temp_len = getSecondQuotePosition(buffer(temp_start):string())
					msg_subtree:add(buffer(temp_start, temp_len), "Subject: " .. buffer(temp_start + 1, temp_len - 2):string())

					msg_count = msg_count + 1

					temp_start = temp_start + temp_len + 2
				end

				msgs_subtree:set_text("Message(s): " .. msg_count)

			-- Fetch command response
			elseif buffer(buffer_len - 2, 1):string() == ")" then
				message_subtree = subtree:add(buffer(4, buffer_len - 5), "Message")

				local temp_start = 5

				-- Sender
				local temp_len = getSecondQuotePosition(buffer(temp_start):string())
				message_subtree:add(buffer(temp_start, temp_len), "Sender: " .. buffer(temp_start + 1, temp_len - 2):string())

				-- Subject
				temp_start = temp_start + temp_len + 1
				temp_len = getSecondQuotePosition(buffer(temp_start):string())
				message_subtree:add(buffer(temp_start, temp_len), "Subject: " .. buffer(temp_start + 1, temp_len - 2):string())

				-- Body
				temp_start = temp_start + temp_len + 1
				temp_len = getSecondQuotePosition(buffer(temp_start):string())
				message_subtree:add(buffer(temp_start, temp_len), "Body: " .. buffer(temp_start + 1, temp_len - 2):string())

			-- Register command response
			elseif buffer(5, 1):string() == "r" then
				subtree:add(buffer(21, buffer_len - 21 - 2), "Registered User: " .. buffer(21, buffer_len - 21 - 2):string())

			-- Login command response
			elseif buffer(5, 1):string() == "u" then
				subtree:add(buffer(21, buffer_len - 21 - 1), "Token: " .. buffer(22, buffer_len - 22 - 2):string())
			else
				subtree:add(buffer(4, buffer_len - 5), "Message Body: " .. buffer(4, buffer_len - 5):string())
			end

		-- --- --
		-- ERR --
		-- --- --
		elseif buffer(1, 3):string() == "err" then
			subtree:add(buffer(1, 3), "Response Status: Error")
			subtree:add(buffer(5, buffer_len - 6), "Message Body: " .. buffer(5, buffer_len - 6):string())
		else
			return 0
		end

	-- -------------- --
	-- Client request --
	-- -------------- --
	else
		subtree:add("Sender: Client")

		local command = substring_up_to(buffer(1, buffer_len - 2):string(), " ")
		local command_len = command:len()

		-- Capitalize first character
		local command_name = command:sub(1,1):upper() .. command:sub(2)
		subtree:add(buffer(1, command_len), "Command Type: " .. command_name)

		-- Parse arguments to a table
		command_args = {}
		command_args_count = 0
		local arg_offset = command_len + 2
		-- Parsing
		while arg_offset < (buffer_len - 1) do
			local a = substring_up_to(buffer(arg_offset, buffer_len - arg_offset - 1):string(), " ")
			if a == nil or a == "" then break end

			command_args[command_args_count] = a
			command_args_count = command_args_count + 1

			arg_offset = arg_offset + a:len() + 1  -- +1 for empty space
		end

		args_tree = subtree:add(buffer(command_len + 2, buffer_len - command_len - 3), "Command Argument(s): " .. command_args_count)

		local arg_offset = command_len + 2

		-- Arguments
		if command == "register" or command == "login" then
			if command_args_count ~= 2 then return end

			local names = { [0] = "Username", "Password" }
			add_args_to_tree(args_tree, buffer, arg_offset, command_args, names, command_args_count)

		elseif command == "logout" or command == "list" then
			if command_args_count ~= 1 then return end

			local names = { [0] = "Token" }
			add_args_to_tree(args_tree, buffer, arg_offset, command_args, names, command_args_count)

		elseif command == "fetch" then
			if command_args_count ~= 2 then return end

			local names = { [0] = "Token", "Message Id" }
			add_args_to_tree(args_tree, buffer, arg_offset, command_args, names, command_args_count)

		elseif command == "send" then
			if command_args_count ~= 4 then return end

			local names = { [0] = "Token", "Recipient", "Subject", "Body" }
			add_args_to_tree(args_tree, buffer, arg_offset, command_args, names, command_args_count)
		else
			return 0
		end
	end
end

local tcp_port = DissectorTable.get("tcp.port")
tcp_port:add("32323", isa_proto)


-- Warning: All tables must be indexable from 0.
function add_args_to_tree(tree, buffer, arg_offset, args, names, count)
	for i = 0, count - 1 do
		tree:add(buffer(arg_offset, args[i]:len()), names[i] .. ": " .. args[i]:sub(2, args[i]:len() - 1))
		arg_offset = arg_offset + args[i]:len() + 1  -- +1 for empty space
	end
end


-- Returns substring up to char position (excluding it)
function substring_up_to(str, char)
	-- Chceck char
	if char == nil or char:len() ~= 1 then
		return nil
	end

	result = ""

	for i = 1, #str do
		local c = str:sub(i,i)
		if c == char then
			return result
		else
			result = result .. c
		end
	end

	return result
end


-- Returns first index of character c in string str or -1 if not found
function firstIndexOf(str, c)
	for i = 1, #str do
		if str:sub(i, i) == c then
			return i
		end
	end

	return -1
end


-- Returns position of second quote (`"`) in the string or -1 if not found
-- Escaped quotes (`\"`) are skipped
function getSecondQuotePosition(str)
	local foundFirst = false

	-- Escaping next character?
	-- Set to true when previous character was unpaired backslash (`\`)
	local escaping = false
	-- Set to true when current character was escaped
	local escaped = false

	for i = 1, #str do
		-- Reset escaping flags if needed
		if escaping and escaped then
			escaping = false
			escaped = false
		end

		if str:sub(i, i) == '\\' and not escaping then
			escaping = true
		elseif str:sub(i, i) == '"' and not escaping then
			if foundFirst then
				return i
			else
				foundFirst = true
			end
		else
			if escaping then
				escaped = true
			end
		end
	end

	return -1
end
