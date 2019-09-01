import google.protobuf 
import hello_pb2

def rpc_server_hello_req(action_id, flag, pb, req_str, value):
    pb_data = hello_pb2.HelloData()
    pb_data.ParseFromString(pb)
    print("rpc call success!") 
    print("rpc call success!!",action_id, flag, pb, req_str, value) 
    print("pb info!",pb_data.name, pb_data.id, pb_data.id_list) 
