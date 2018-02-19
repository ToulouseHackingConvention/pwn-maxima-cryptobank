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

up: build
	docker run -d -p 5555:5555 --name $(CTN) $(IMG)

down:
	-docker rm -f $(CTN)

logs:
	docker logs -f $(CTN)

clean: down
	-docker rmi $(IMG)
	rm -rf export

.PHONY: build export up down logs clean
