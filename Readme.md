# python-rpc
一个Python的rpc模块
需要在python3.5以上版本

#使用方法
在rpc目录下增加xml的rpc配置文件

    <root>
        <!-- 服务器执行 -->
        <server>
            <!-- 远程函数名字 -->
            <function>rpc_server_hello_req</function>
            <!-- 远程模块 -->
            <path>scripts.hello</path>
            <args>
                <!-- 参数支持的各种类型 -->
                <arg>RPC_INT</arg>
                <arg>RPC_BOOL</arg>
                <arg>RPC_PB</arg>
                <arg>RPC_STRING</arg>
                <arg>RPC_FLOAT</arg>
            </args>
            <deamon>1</deamon>
        </server>
        <!-- 客户端执行 -->
        <client>
            <!-- 远程函数名字 -->
            <function>rpc_clent_hello_resp</function>
            <args>
                <!-- 操作ID -->
                <arg>RPC_INT</arg>
                <arg>RPC_PB</arg>
                <arg>RPC_STRING</arg>
            </args>
        </client>
    </root>

#例子
用户代码 test_rpc.py

    import rpc
    import google.protobuf 
    import sys
    sys.path.append("./proto/output")
    import hello_pb2
    
    hello_data = hello_pb2.HelloData()
    hello_data.id = 1
    hello_data.name = "消息"
    hello_data.id_list.append(1)
    hello_data.id_list.append(2)
    
    print(type(hello_data))
    print(type(hello_data.SerializeToString()))
    
    packer_buf = rpc.pack("rpc_server_hello_req", (123, True,  hello_data.SerializeToString(), "abc", 1.3333))
    print(packer_buf)

    #unpack_obj = rpc.unpack(packer_buf)
    #print("unpack", unpack_obj)
    
    rpc.rpc_call(packer_buf)

远程代码 scripts.hello.py
    
    import google.protobuf 
    import hello_pb2
    
    def rpc_server_hello_req(action_id, flag, pb, req_str, value):
        pb_data = hello_pb2.HelloData()
        pb_data.ParseFromString(pb)
        print("rpc call success!!",action_id, flag, pb, req_str, value) 
        print("pb info!",pb_data.name, pb_data.id, pb_data.id_list) 
