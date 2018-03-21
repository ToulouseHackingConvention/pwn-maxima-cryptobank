# Crypto Bank

- Author: Maxime Arthaud < maxime@arthaud.me >
- Type: pwn

## Description

Hello Sir,

I work for the US Government. I have reasons to believe that the THC Crypto Bank
is conducting illegal activities. And they don't pay their taxes..

One of my coworkers told me you helped us in the past.
Could you do something for me? Break into their server or something?

If it can help, I heard they use some top-notch security tool..

nc localhost 5555

## How to run the challenge

First, make sure you have [docker](https://www.docker.com/) installed.

Then, run `make build` to build the image for the challenge.

Run `make export` to get the files provided to the challengers. It should create a directory `export` containing the files.

Finally, run `make run` to start a container running the challenge.
You should be able to connect to localhost:5555.

Happy Hacking!

## Writeup

See [WRITEUP.md](solution/WRITEUP.md)

## Files provided to the challengers

- cryptobank
- cryptobank.c
- libc.so.6

## Build the docker image

`make build`

## Run the docker container

`make run`

## Get the provided files

`make export`

## Update the flag

Update `src/flag` then `make build`

## Show container logs

`make logs`

## Clean

* `make clean` removes the container and the provided files
* `make clean-all` removes the container, the docker image and the provided files
