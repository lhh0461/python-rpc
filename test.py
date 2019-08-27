import rpc
import google.protobuf 
import sys
sys.path.append("./proto/output")
import test_pb2

mail_data = test_pb2.MailData()
mail_data.id = 1
mail_data.title = "大王"
mail_data.item_list.append(1)
mail_data.item_list.append(2)

print(type(mail_data))
print(type(mail_data.SerializeToString()))

packer_buf = rpc.pack("rpc_clent_test", (1111, mail_data.SerializeToString()))
print(packer_buf)
