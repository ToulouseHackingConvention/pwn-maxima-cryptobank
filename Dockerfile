FROM debian:stretch

RUN echo "deb http://apt.llvm.org/stretch/ llvm-toolchain-stretch-5.0 main" >> /etc/apt/sources.list

# install clang 5.0
RUN apt-get update && \
    apt-get -y upgrade && \
    apt-get install -y --allow-unauthenticated clang-5.0 socat

RUN useradd \
      --shell /bin/nologin \
      --home-dir / \
      chall

ADD src/cryptobank.c /srv/cryptobank.c
ADD src/flag /flag

# compile cryptobank.c
RUN clang-5.0 \
      -g \
      -fno-omit-frame-pointer \
      -fsanitize=undefined,integer,nullability \
      -fno-sanitize-recover=undefined,integer,nullability \
      -fsanitize=address \
      -fsanitize-address-use-after-scope \
      -O1 \
      /srv/cryptobank.c \
      -o /srv/cryptobank

# clean-up
RUN chmod 444 /flag && \
    rm -f /srv/cryptobank.c && \
    apt-get purge -y --auto-remove clang-5.0

EXPOSE 5555

ENV ASAN_OPTIONS="detect_leaks=0:strict_string_checks=1:detect_stack_use_after_return=1:check_initialization_order=1:strict_init_order=1"

CMD socat TCP-LISTEN:5555,fork EXEC:"/srv/cryptobank",su=chall,stderr
