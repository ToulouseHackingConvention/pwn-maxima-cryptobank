# Crypto Bank

- Author: Maxime Arthaud < maxime@arthaud.me >
- Type: pwn

## Description

Hello Sir,

I work for the US Government. I have reasons to believe that the THC Crypto Bank
is conducting illegal activities. And they don't pay their taxes..

They told me you helped us in the past. Could you hack them for me?

If it can help, I heard they use some top-notch security tool..

nc localhost 5555

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
