# NewYorkCoin 1.3
=========================

NewYorkCoin  1.3 is a complete re-architecture of NewYorkCoin, changing from
using the Litecoin client as its base, to Dogecoin. It's still NewYorkCoin,
with the same Scrypt PoW algorithm, same reward schedule, but there are a
lot of changes under the hood.


newyorkcoin-cli
------------

Where previously commands were sent to newyorkcoind by running
"newyorkcoind <command>", 1.3 adopts the model from Bitcoin Core where there is
a separate "newyorkcoin-cli" executable which is used instead. This avoids the risk
of accidentally trying to start two daemons at the same time, for example.


Transaction malleability-related fixes
--------------------------------------

Fixes for risk-cases involving transaction malleability have been added; this
is particularly important for any merchants or exchanges using the built-in
wallet system.

Testnet
-------

As mentioned at the start of this document, the alpha-client is for use with the
NewYorkCoin testnet only. This is an alternative NewYorkCoin blockchain which is
not used for real transactions, and instead is intended for testing of experimental
clients. Wallets and addresses are incompatible with the normal NewYorkCoin
network, in order to isolate the two.

As "TestNYCoin" is essentially valueless, it can be acquired easily either by mining: in the debug console of the QT Wallet: setgenerate true 2
