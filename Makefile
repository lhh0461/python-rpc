.PHONY : rpc 
rpc:
	cd proto/def && protoc --proto_path=./ * --python_out=../output
ext:
	gcc -I /usr/include/python3.5 -fpic --shared -o spam.so Module.c
