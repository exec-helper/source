FROM debian:testing
RUN apt-get update && apt-get install --yes --no-install-recommends meson cmake make libboost-dev libboost-program-options-dev libboost-log-dev libyaml-cpp-dev liblua5.3-dev pkg-config g++ git python3 catch sudo curl libreadline-dev python3-sphinx python3-sphinx-rtd-theme && apt-get clean --yes && rm -rf /var/lib/apt/lists/*

RUN ln -s python3 /usr/bin/python

# Install gitchangelog using the standalone installer
RUN curl -sSL https://raw.githubusercontent.com/vaab/gitchangelog/master/src/gitchangelog/gitchangelog.py > /usr/local/bin/gitchangelog && chmod +x /usr/local/bin/gitchangelog
