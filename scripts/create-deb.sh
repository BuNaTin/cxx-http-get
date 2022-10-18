
rm -rf build/ && mkdir -p build/ && cd build && cmake ..\
&& cpack -G DEB --config cxx-http-get.cpack.cmake\
&& mv *.deb ../bin && cd .. && rm -rf build/
