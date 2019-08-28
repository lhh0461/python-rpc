import rpc
import google.protobuf 
import sys
sys.path.append("./proto/output")
import hello_pb2

hello_data = hello_pb2.HelloData()
hello_data.id = 1
hello_data.name = "大王"
hello_data.id_list.append(1)
hello_data.id_list.append(2)

print(type(hello_data))
print(type(hello_data.SerializeToString()))

packer_buf = rpc.pack("rpc_clent_hello_resp", (123, True,  hello_data.SerializeToString(), "abc", 1.3333))
print(packer_buf)
unpack_obj = rpc.unpack(packer_buf)
print("unpack", unpack_obj)
