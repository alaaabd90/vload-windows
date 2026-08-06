module grpc_server

go 1.25.0

require (
	github.com/grpc-ecosystem/go-grpc-middleware v1.3.0
	github.com/matsuridayo/libneko v1.0.0 // replaced
	google.golang.org/grpc v1.83.0
	google.golang.org/protobuf v1.36.11
)

require (
	cloud.google.com/go/compute/metadata v0.9.0 // indirect
	golang.org/x/net v0.55.0 // indirect
	golang.org/x/sys v0.45.0 // indirect
	golang.org/x/text v0.37.0 // indirect
	google.golang.org/genproto v0.0.0-20211223182754-3ac035c7e7cb // indirect
)

exclude cloud.google.com/go v0.34.0

replace github.com/matsuridayo/libneko v1.0.0 => ../../../libneko
