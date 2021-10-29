-- ISA project - a protocol dissector
-- Peter Urgoš (xurgos00)
-- 11/2021

isa_proto = Proto("ISA", "ISA Protocol")

msg_len = ProtoField.int32("isa_proto.msg_len", "Message Length", base.DEC)
msg_type = ProtoField.string("isa_proto.msg_type", "Message Type")
response_status = ProtoField.string("isa_proto.response_status", "Response Status")
msg_body = ProtoField.string("isa_proto.msg_body", "Message Body")
cmd_type = ProtoField.string("isa_proto.cmd_type", "Command Type")
cmd_args_count = ProtoField.int32("isa_proto.cmd_args_count", "Arguments Count", base.DEC)

isa_proto.fields = { msg_len, msg_type, response_status, msg_body, cmd_type, cmd_args_count }

function isa_proto.dissector(buffer, pinfo, tree)
	local buffer_len = buffer:len()

	if buffer_len == 0 then return 0 end

	-- Set column name
	pinfo.cols.protocol = isa_proto.name

	-- Add root node of this protocol to the tree
	local subtree = tree:add(isa_proto, buffer(), "ISA Protocol Data")

	subtree:add(msg_len, buffer_len)

	-- Server response
	if (pinfo.src_port == 32323) then
		subtree:add(msg_type, "Response")

		if buffer(1, 2):string() == "ok" then
			subtree:add(response_status, buffer(1, 2), "Ok")
			subtree:add(msg_body, buffer(4, buffer_len - 2 - 3))
		elseif buffer(1, 3):string() == "err" then
			subtree:add(response_status, buffer(1, 3), "Error")
			subtree:add(msg_body, buffer(5, buffer_len - 2 - 4))
		else
			return 0
		end
	
	-- Client request
	else
		subtree:add(msg_type, "Request")

		local command = substring_up_to(buffer(1, buffer_len - 2):string(), " ")
		local command_len = command:len()

		-- Capitalize first character
		local command_name = command:sub(1,1):upper()..command:sub(2)
		subtree:add(cmd_type, buffer(1, command_len), command_name)
		args_tree = subtree:add(buffer(command_len + 2, buffer_len - command_len - 3), "Command Argument(s)")

		-- Parse arguments to a table
		command_args = {}
		command_args_count = 0
		local arg_offset = command_len + 2
		-- Parsing
		while arg_offset < (buffer_len - 1) do
			local a = substring_up_to(buffer(arg_offset, buffer_len - arg_offset - 1):string(), " ")
			if a == nil or a == "" then break end
			--print(command_args_count .. " : " .. a)
			
			command_args[command_args_count] = a
			command_args_count = command_args_count + 1

			arg_offset = arg_offset + a:len() + 1  -- +1 for empty space
		end

		-- Arguments count
		args_tree:add(cmd_args_count, command_args_count)

		local arg_offset = command_len + 2

		-- Arguments
		if command == "register" or command == "login" then
			if command_args_count ~= 2 then return end

			local names = { [0] = "Username", "Password" }
			add_args_to_tree(args_tree, buffer, arg_offset, command_args, names, command_args_count)
		
		elseif command == "logout" or command == "list" then
			if command_args_count ~= 1 then return end

			local names = { [0] = "Token" }
			add_args_to_tree(tree, buffer, arg_offset, command_args, names, command_args_count)
		
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

print("ISA dissector loaded\n")

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
