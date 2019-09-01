# python-rpc
一个Python的rpc模块

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
