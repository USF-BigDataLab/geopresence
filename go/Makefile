
all : c_libs protobuf build

c_libs :
	$(MAKE) -C ../c/
	$(MAKE) -C ../c/ shared_lib
	cp ../c/libcount.a ../c/libgeode.so ../go/geode/
	$(MAKE) -C ../c/ clean

protobuf :
	protoc --go_out=./ proto/*.proto

build :
	go build
