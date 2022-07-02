FROM ubuntu:22.04

RUN DEBIAN_FRONTEND="noninteractive" apt-get update && apt-get -y install tzdata

RUN apt-get update

# Build Tools
RUN apt-get install -y git gdb gcc g++ make ninja-build cmake autoconf automake

# Deps
RUN apt-get install -y libglew-dev
RUN apt-get install -y libglfw3-dev
RUN apt-get install -y libglm-dev

# Complete
RUN apt-get clean
