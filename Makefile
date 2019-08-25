rpc:
	cd proto/def && protoc --proto_path=./ * --python_out=../output
