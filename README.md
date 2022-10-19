# cxx-http-get #

Simple, single-thread, concise sharing folder http server, intended for everyday use, has a lot of impact in embedded cause of buffering request reading

## Features ##

 - [x] Share folder
 - [x] Able custom handlers
 - [ ] Thread-pool
 - [ ] Resizable buffer & unbuffered version
 - [ ] Fully parsed request headers

## Arguments ##

 - help [h] - Print help message
 - port [p] - Bind port, default is 8080
 - folder [f] - Sharing folder, default is ./
 - sigint_cnt [sig] - Sigint handling count before abort

## Installing ##

Create deb packages requers cmake & cpack - just run ./scripts/create-deb.sh

## Work with server ##

To get and put files use
1. Web browser
   - `get file` - put `ip:port` to address field
2. Curl
   - `get file` - curl http://ip:port/path_to_file --output path_to_save_file
   - `put file` - curl -X POST -H "Content-Type:application/octet-stream" --data-binary @path_to_file http://ip:port/path_to_put
