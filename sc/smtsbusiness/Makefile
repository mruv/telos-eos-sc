ECPP=eosio-cpp
ARGS=-w --abigen -I ./

build: clean
	$(ECPP) $(ARGS) -o smtsbusiness.wasm smtsbusiness.cpp

clean:
	rm -f ./*.wasm ./*.abi
