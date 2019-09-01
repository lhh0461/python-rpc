.PHONY : rpc 
rpc:
	cd proto/def && protoc --proto_path=./ * --python_out=../output
ext:
	g++ -g -std=c++11 -I /usr/include/python3.5 -fpic --shared -o rpc.so Script.cpp Module.cpp Rpc.cpp -I./3rd/msgpack/include/  -I ./3rd/tinyxml2/include -L ./3rd/tinyxml2/ -ltinyxml2
