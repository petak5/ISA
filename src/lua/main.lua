-- ISA project - a protocol dissector
-- Peter Urgoš (xurgos00)
-- 11/2021

isa_proto = Proto("ISA", "ISA Protocol")

msg_len = ProtoField.int32("isa_proto.msg_len", "Message Length", base.DEC)
msg_type = ProtoField.string("isa_proto.msg_type", "Message Type")
msg_status = ProtoField.string("isa_proto.msg_status", "Message Status")
msg_body = ProtoField.string("isa_proto.msg_body", "Message Body")
cmd_type = ProtoField.string("isa_proto.cmd_type", "Command Type")

isa_proto.fields = { msg_len, msg_type, msg_status, msg_body, cmd_type }

function isa_proto.dissector(buffer, pinfo, tree)
	if buffer:len() == 0 then
		return end

	-- Set column name
	pinfo.cols.protocol = isa_proto.name

	-- Add root node of this protocol to the tree
	local subtree = tree:add(isa_proto, buffer(), "ISA Protocol Data")

	subtree:add(msg_len, buffer:len())

	-- Server response
	if (pinfo.src_port == 32323) then
		subtree:add(msg_type, "Server  response")

		if buffer(1, 2):string() == "ok" then
			subtree:add(msg_status, buffer(1, 2), "Ok")
			subtree:add(msg_body, buffer(4, buffer:len() - 2 - 3))
		elseif buffer(1, 3):string() == "err" then
			subtree:add(msg_status, buffer(1, 3), "Error")
			subtree:add(msg_body, buffer(5, buffer:len() - 2 - 4))
		else
			subtree:add(msg_status, "Unknown")
		end
	
	-- Client request
	else
		subtree:add(msg_type, "Client request")


		local command = substring_up_to(buffer(1, buffer:len() - 2):string(), " ")
		subtree:add(cmd_type, buffer(1, command:len()), command)
	end

end

local tcp_port = DissectorTable.get("tcp.port")
tcp_port:add("32323", isa_proto)

print("ISA dissector loaded")

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
