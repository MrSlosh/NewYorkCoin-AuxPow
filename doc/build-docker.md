## DOCKER BUILD NOTES

Some notes on how to build and run NewYorkCoin inside a Docker Container

### Dependency

Install docker on your PC, laptop or server via: https://docs.docker.com/install/


### To Build

```bash
cd /work
sudo docker build -t newyorkcoin-pow:0.1 .
```

Should show you some output like:

```bash
sudo docker build -t newyorkcoin-pow:0.1 .
Sending build context to Docker daemon  1.195GB
Step 1/10 : FROM ubuntu:14.04
...
...
...
---> b45501915bb0
Step 10/10 : EXPOSE 17020 27020 18823 28823
---> Running in fdb0e6fb8547
Removing intermediate container fdb0e6fb8547
---> ec2fc27f37ca
Successfully built ec2fc27f37ca
Successfully tagged newyorkcoin-pow:0.1
root@rancher:/work/NewYorkCoin-AuxPow# sudo docker build -t newyorkcoin-pow:0.1 .
```

This will build the newyorkcoin daemon insite a container tagged with a number. This makes it easier to upgrade to a newer codebase version.

### Start the container on either mainnet or testnet:

Run docker with the config provided in either docker/mainnet of docker/testnet. Feel free to modifiy the files or create a new on in a different path

* Mainnet:
`docker run -d -v $(pwd)/docker/mainnet:/root/.newyorkcoin/ newyorkcoin-pow:0.1`

* Testnet:
`docker run -d -v $(pwd)/docker/testnet:/root/.newyorkcoin/ newyorkcoin-pow:0.1`

With: `-v`: persist the data in your host machine.
