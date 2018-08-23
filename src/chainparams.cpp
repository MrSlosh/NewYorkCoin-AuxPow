// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2015 The Dogecoin Core developers
// Copyright (c) 2018 The New York Coin Community developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"

#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

using namespace std;

#include "chainparamsseeds.h"

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

class CMainParams : public CChainParams {
protected:
    Consensus::Params digishieldConsensus;
    Consensus::Params auxpowConsensus;
public:
    CMainParams() {
        strNetworkID = "main";

        // Blocks 0 - 4799999 are conventional difficulty calculation
        consensus.nSubsidyHalvingInterval = 500000;
        consensus.nMajorityEnforceBlockUpgrade = 1500;
        consensus.nMajorityRejectBlockOutdated = 1900;
        consensus.nMajorityWindow = 2000;
        consensus.powLimit = uint256S("0x00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 20;
        consensus.nPowTargetTimespan = 2 * 60 * 60; // pre-digishield: 2 hours
        consensus.nPowTargetSpacing = 30; // 30 seconds
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowAllowDigishieldMinDifficultyBlocks = false;
        consensus.fSimplifiedRewards = false;
        consensus.nAuxpowChainId = 0x7C1; // 1985
        consensus.fStrictChainId = true;
        consensus.fAllowLegacyBlocks = true;
        consensus.fAllowAuxPow = false;
        consensus.nHeightEffective = 0;
        consensus.fDigishieldDifficultyCalculation = false;
        consensus.nCoinbaseMaturity = 30;

        // Blocks 4800000 are Digishield with AuxPoW
        digishieldConsensus = consensus;
        digishieldConsensus.nHeightEffective = 4800000;
        digishieldConsensus.fAllowLegacyBlocks = false;
        digishieldConsensus.fSimplifiedRewards = true;
        digishieldConsensus.fDigishieldDifficultyCalculation = true;
        digishieldConsensus.nPowTargetTimespan = 60; // post-digishield: 60 seconds
        digishieldConsensus.nCoinbaseMaturity = 240;

        // Blocks 4800000+ are AuxPoW
        auxpowConsensus = digishieldConsensus;
        auxpowConsensus.nHeightEffective = 4800000;
        auxpowConsensus.fAllowLegacyBlocks = false;
        auxpowConsensus.fAllowAuxPow = true;

        // Assemble the binary search tree of consensus parameters
        pConsensusRoot = &digishieldConsensus;
        digishieldConsensus.pLeft = &consensus;
        digishieldConsensus.pRight = &auxpowConsensus;

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xc0;
        pchMessageStart[1] = 0xc0;
        pchMessageStart[2] = 0xc0;
        pchMessageStart[3] = 0xc0;
        vAlertPubKey = ParseHex("04AFC6B9D279C647C67C250BC4C68F5E9BD714867D8D45B6E54FEEFFA6CB9074EE7CBF69EB8FF388304604C2E00661601502B42E3CF71E934B4A5EAB0D5B209BFD");
        nDefaultPort = 17020;
        nMinerThreads = 0;
        nPruneAfterHeight = 100000;

        /**
         * Build the genesis block. Note that the output of its generation
         * transaction cannot be spent since it did not originally exist in the
         * database.
         *
         * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1386325540, nBits=0x1e0ffff0, nNonce=99943, vtx=1)
         *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
         *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
         *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
         *   vMerkleTree: 4a5e1e
         */
        const char* pszTimestamp = "A Coin for New York City";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 88 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("040184710fa689ad5023690c80f3a49c8f13f8d45b8c857fbcbc8bc4a8e4d3eb4b10f4d4604fa08dce601aaf0f470216fe1b51850b4acf21b179c45070ac7b03a9") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock.SetNull();
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion.SetGenesisVersion(1);
        genesis.nTime    = 1394102925;
        genesis.nBits    = 0x1e0ffff0;
        genesis.nNonce   = 2482334;

        consensus.hashGenesisBlock = genesis.GetHash();
        digishieldConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
        auxpowConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
        assert(consensus.hashGenesisBlock == uint256S("0x5597f25c062a3038c7fd815fe46c67dedfcb3c839fbc8e01ed4044540d08fe48"));
        assert(genesis.hashMerkleRoot == uint256S("0x2bad42ac6e0ccc4808d8df0fd50ac8634eea335b1412b1ef52864b430a87b262"));

        vSeeds.push_back(CDNSSeedData("nycoin.money", "dnsseed.nycoin.money"));
        vSeeds.push_back(CDNSSeedData("nycoin.community", "dnsseed.nycoin.community"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,60);  // 0x1e
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,22);  // 0x16
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,188); // 0xbc
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xb2)(0x1e).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xad)(0xe4).convert_to_container<std::vector<unsigned char> >();

        //TODO: fix this for newyorkcoin -- plddr
        //vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));
        vFixedSeeds.clear();


        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (Checkpoints::CCheckpointData) {
            boost::assign::map_list_of


            (  10000, uint256S("0x132e14f7d82b659329ac95300413beba2c00f9e3d1b137533a093fce18d3febd"))
            (  100000, uint256S("0x495da2e0cffa0ad6c0fe83c2678e2c714e024ed009abcdb24728d306b599232f"))
            ( 155511, uint256S("0x0cd7a29253710ebf4c71c473f61e586b044a5da64380e424b63c9f45c89b7cde"))
            ( 500000, uint256S("0xb9db8c11eacd9921fb09ad149fc9b7cf41e429e759f8304043398d7ed1067952"))
            ( 1000000, uint256S("0x157fc4df4e7594abae3487c554bbea91cd70a1014faf7ae7b5d3ee4d9da80226"))
            ( 1500000, uint256S("0xc185fa9930597d386977969894cecaec21547589efdf756d533824f86244cda1"))
            ( 2000000, uint256S("0xde3f57919d2048c915e369642f6216aed78f5f0c9a59e45763a09d97f483fa2a"))
            ( 2500000, uint256S("0x93896d159dbc68e7ac109f4fa7e05365cbb08d78b6fe3957c4a330a878bf2e2a"))
            (3000000,  uint256S("0x66e6dcb49370062537c1f6abf655ffbbc53ba4851ef00081aa2e4be1e2903ba7"))
          (3500000,  uint256S("0x3f1a97f68ce8eaf38fc0c56868b3eb98ccb67d14bff4e78afb91d82cba853ddf"))
          (3938415,  uint256S("0xe1fa41f6fe8d2785d89b0468e13e4c450493e5356c024a098c5b727ca89138ee"))
          (4500000, uint256S("0xdd86fad58b3fa5d83a15a18df1cc20cdcdb1b2cf5d2d702e0c60bbb7d4602fb1")),

            1514765144, // * UNIX timestamp of last checkpoint block
            5141422,   // * total number of transactions between genesis and last checkpoint
                        //   (the tx=... number in the SetBestChain debug.log lines)
            7000.0     // * estimated number of transactions per day after checkpoint
        };
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams {
private:
    Consensus::Params minDifficultyConsensus;
public:
    CTestNetParams() {
        strNetworkID = "test";

        // Blocks 0 - 144999 are pre-Digishield
        consensus.nHeightEffective = 0;
        consensus.nSubsidyHalvingInterval = 1722;
        consensus.nPowTargetTimespan = 2 * 60 * 60; // pre-digishield:2 hours
        consensus.fDigishieldDifficultyCalculation = false;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowAllowDigishieldMinDifficultyBlocks = false;
        consensus.nMajorityEnforceBlockUpgrade = 501;
        consensus.nMajorityRejectBlockOutdated = 750;
        consensus.nMajorityWindow = 1000;
        consensus.fStrictChainId = false;
        consensus.fAllowLegacyBlocks = true;
        consensus.fAllowAuxPow = false;

        // Reset links before we copy parameters
        consensus.pLeft = NULL;
        consensus.pRight = NULL;

        // Blocks 145000 - 157499 are Digishield without minimum difficulty on all blocks
        digishieldConsensus = consensus;
        digishieldConsensus.nHeightEffective = 15500;
        digishieldConsensus.nPowTargetTimespan = 60; // post-digishield: 1 minute
        digishieldConsensus.fDigishieldDifficultyCalculation = true;
        digishieldConsensus.fSimplifiedRewards = true;
        digishieldConsensus.fPowAllowMinDifficultyBlocks = false;
        digishieldConsensus.nCoinbaseMaturity = 30;

        // Blocks 157500 - 158099 are Digishield with minimum difficulty on all blocks
        minDifficultyConsensus = digishieldConsensus;
        minDifficultyConsensus.nHeightEffective = 15500;
        minDifficultyConsensus.fPowAllowDigishieldMinDifficultyBlocks = true;
        minDifficultyConsensus.fPowAllowMinDifficultyBlocks = true;

        // Enable AuxPoW at 158100
        auxpowConsensus = minDifficultyConsensus;
        auxpowConsensus.nHeightEffective = 15500;
        auxpowConsensus.fPowAllowDigishieldMinDifficultyBlocks = true;
        auxpowConsensus.fAllowLegacyBlocks = false;
        auxpowConsensus.fAllowAuxPow = true;

        // Assemble the binary search tree of parameters
        pConsensusRoot = &digishieldConsensus;
        digishieldConsensus.pLeft = &consensus;
        digishieldConsensus.pRight = &minDifficultyConsensus;
        minDifficultyConsensus.pRight = &auxpowConsensus;

        pchMessageStart[0] = 0xac;
        pchMessageStart[1] = 0xb1;
        pchMessageStart[2] = 0xc5;
        pchMessageStart[3] = 0xdc;
        vAlertPubKey = ParseHex("04302390343f91cc401d56d68b123028bf52e5fca1939df127f63c6467cdf9c8e2c14b61104cf817d0b780da337893ecc4aaff1309e536162dabbdb45200ca2b0a");
        nDefaultPort = 27020;
        nMinerThreads = 0;
        nPruneAfterHeight = 1000;

        //! Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1394101189;
        genesis.nNonce = 1556996;
        consensus.hashGenesisBlock = genesis.GetHash();
        digishieldConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
        minDifficultyConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
        auxpowConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
        assert(consensus.hashGenesisBlock == uint256S("0x24463e4d3c625b0a9059f309044c2cf0d7e196cf2a6ecce901f24f681be33c8f"));

        vFixedSeeds.clear();
        vSeeds.clear();
        //vSeeds.push_back(CDNSSeedData("jrn.me.uk", "testseed.jrn.me.uk"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,113); // 0x71
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196); // 0xc4
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,241); // 0xf1
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xb2)(0x1e).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xad)(0xe4).convert_to_container<std::vector<unsigned char> >();

        //TODO: fix this for newyorkcoin -- plddr
        //vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        checkpointData = (Checkpoints::CCheckpointData) {
            boost::assign::map_list_of
            ( 0, uint256S("0x24463e4d3c625b0a9059f309044c2cf0d7e196cf2a6ecce901f24f681be33c8f"))
            ,
            1440601451, // * UNIX timestamp of last checkpoint block
            1119061,    // * total number of transactions between genesis and last checkpoint
            1000        // * estimated number of transactions per day after checkpoint
        };

    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.nPowTargetTimespan = 2 * 60 * 60; // pre-digishield: 4 hours
        consensus.nPowTargetSpacing = 1; // regtest: 1 second blocks
        consensus.powLimit = uint256S("0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1;
        consensus.fStrictChainId = true;
        consensus.fAllowLegacyBlocks = true;
        consensus.fAllowAuxPow = false;
        consensus.fSimplifiedRewards = true;
        consensus.nCoinbaseMaturity = 10; // For easier testability in RPC tests

        // Reset links before we copy parameters
        consensus.pLeft = NULL;
        consensus.pRight = NULL;

        digishieldConsensus = consensus;
        digishieldConsensus.nHeightEffective = 20;
        digishieldConsensus.nPowTargetTimespan = 1; // // regtest: also retarget every second in digishield mode, for conformity
        digishieldConsensus.fDigishieldDifficultyCalculation = true;

        auxpowConsensus = digishieldConsensus;
        auxpowConsensus.fAllowLegacyBlocks = false;
        auxpowConsensus.fAllowAuxPow = true;
        auxpowConsensus.nHeightEffective = 20;

        // Assemble the binary search tree of parameters
        digishieldConsensus.pLeft = &consensus;
        digishieldConsensus.pRight = &auxpowConsensus;
        pConsensusRoot = &digishieldConsensus;

        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xda;
        nMinerThreads = 1;
        genesis.nTime = 1528384153;
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 2;
        consensus.hashGenesisBlock = genesis.GetHash();
        digishieldConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
        auxpowConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
        nDefaultPort = 18444;
        assert(consensus.hashGenesisBlock == uint256S("0xa7ff2d7850dc3b85db38c38f265a0eaa3771b4ba1237127a82ac748a2bba27b3"));
        nPruneAfterHeight = 1000;

        vFixedSeeds.clear(); //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Regtest mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (Checkpoints::CCheckpointData){
            boost::assign::map_list_of
            ( 0, uint256S("0xa7ff2d7850dc3b85db38c38f265a0eaa3771b4ba1237127a82ac748a2bba27b3")),
            0,
            0,
            0
        };
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams &Params(CBaseChainParams::Network network) {
    switch (network) {
        case CBaseChainParams::MAIN:
            return mainParams;
        case CBaseChainParams::TESTNET:
            return testNetParams;
        case CBaseChainParams::REGTEST:
            return regTestParams;
        default:
            assert(false && "Unimplemented network");
            return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network) {
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
