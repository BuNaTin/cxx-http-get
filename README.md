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
