FROM debian:testing
RUN apt-get update
RUN apt-get install --yes cmake make libboost-dev libboost-program-options-dev libboost-filesystem-dev libboost-log-dev libyaml-cpp-dev pkg-config g++ git doxygen python-pip catch sudo
RUN pip install gitchangelog
