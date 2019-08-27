.PHONY : rpc 
rpc:
	cd proto/def && protoc --proto_path=./ * --python_out=../output
ext:
	g++ -std=c++11 -I /usr/include/python3.5 -fpic --shared -o rpc.so Module.cpp Util.cpp Rpc.cpp -I./msgpack/include/  -I ./tinyxml2/ -L ./tinyxml2/ -ltinyxml2
