FROM debian:testing
RUN apt-get update && apt-get install --yes cmake make libboost-dev libboost-program-options-dev libboost-log-dev libyaml-cpp-dev libmsgsl-dev pkg-config g++ git doxygen python catch sudo curl && apt-get clean --yes

# Install gitchangelog using the standalone installer
RUN curl -sSL https://raw.githubusercontent.com/vaab/gitchangelog/master/src/gitchangelog/gitchangelog.py > /usr/local/bin/gitchangelog && chmod +x /usr/local/bin/gitchangelog
