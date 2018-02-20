IMG = pwn-maxima-cryptobank
CTN = cryptobank

build:
	docker build -t $(IMG) .

export: build
	mkdir -p export
	cp src/cryptobank.c export/cryptobank.c
	docker run --rm --entrypoint cat $(IMG) /srv/cryptobank > export/cryptobank
	chmod +x export/cryptobank
	docker run --rm --entrypoint cat $(IMG) /lib/x86_64-linux-gnu/libc.so.6 > export/libc.so.6
	chmod +x export/libc.so.6

run:
	docker run -d -p 5555:5555 --name $(CTN) $(IMG)

stop:
	-docker stop $(CTN)

logs:
	-docker logs -f $(CTN)

clean: stop
	-docker rm $(CTN)
	rm -rf export

clean-all:
	-docker rmi $(IMG)

.PHONY: build run logs stop export clean clean-all
