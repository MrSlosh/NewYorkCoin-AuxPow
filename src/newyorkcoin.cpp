// Copyright (c) 2015 The newyorkcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/random/uniform_int.hpp>
#include <boost/random/mersenne_twister.hpp>

#include "arith_uint256.h"
#include "newyorkcoin.h"
#include "main.h"
#include "util.h"

int static generateMTRandom(unsigned int s, int range)
{
    boost::mt19937 gen(s);
    boost::uniform_int<> dist(1, range);
    return dist(gen);
}

// NewYorkCoin: Normally minimum difficulty blocks can only occur in between
// retarget blocks. However, once we introduce Digishield every block is
// a retarget, so we need to handle minimum difficulty on all blocks.
bool AllowDigishieldMinDifficultyForBlock(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    // check if the chain allows minimum difficulty blocks
    if (!params.fPowAllowMinDifficultyBlocks)
        return false;

    // check if the chain allows minimum difficulty blocks on recalc blocks
    if (pindexLast->nHeight < 157500)
    // if (!params.fPowAllowDigishieldMinDifficultyBlocks)
        return false;

    // Allow for a minimum block time if the elapsed time > 2*nTargetSpacing
    return (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2);
}

unsigned int CalculateNewYorkCoinNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    int nHeight = pindexLast->nHeight + 1;
    const int64_t retargetTimespan = params.nPowTargetTimespan;
    const int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    int64_t nModulatedTimespan = nActualTimespan;
    int64_t nMaxTimespan;
    int64_t nMinTimespan;

    if (params.fDigishieldDifficultyCalculation) //DigiShield implementation - thanks to RealSolid & WDC for this code
    {
        // amplitude filter - thanks to daft27 for this code
        nModulatedTimespan = retargetTimespan + (nModulatedTimespan - retargetTimespan) / 8;

        nMinTimespan = retargetTimespan - (retargetTimespan / 4);
        nMaxTimespan = retargetTimespan + (retargetTimespan / 2);
    } else if (nHeight > 10000) {
        nMinTimespan = retargetTimespan / 4;
        nMaxTimespan = retargetTimespan * 4;
    } else if (nHeight > 5000) {
        nMinTimespan = retargetTimespan / 8;
        nMaxTimespan = retargetTimespan * 4;
    } else {
        nMinTimespan = retargetTimespan / 16;
        nMaxTimespan = retargetTimespan * 4;
    }

    // Limit adjustment step
    if (nModulatedTimespan < nMinTimespan)
        nModulatedTimespan = nMinTimespan;
    else if (nModulatedTimespan > nMaxTimespan)
        nModulatedTimespan = nMaxTimespan;

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew;
    arith_uint256 bnOld;
    bnNew.SetCompact(pindexLast->nBits);
    bnOld = bnNew;
    bnNew *= nModulatedTimespan;
    bnNew /= retargetTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

bool CheckAuxPowProofOfWork(const CBlockHeader& block, const Consensus::Params& params)
{
    /* Except for legacy blocks with full version 1, ensure that
       the chain ID is correct.  Legacy blocks are not allowed since
       the merge-mining start, which is checked in AcceptBlockHeader
       where the height is known.  */
    if (!block.nVersion.IsLegacy() && params.fStrictChainId && block.nVersion.GetChainId() != params.nAuxpowChainId)
        return error("%s : block does not have our chain ID"
                     " (got %d, expected %d, full nVersion %d)",
                     __func__,
                     block.nVersion.GetChainId(),
                     params.nAuxpowChainId,
                     block.nVersion.GetFullVersion());

    /* If there is no auxpow, just check the block hash.  */
    if (!block.auxpow) {
        if (block.nVersion.IsAuxpow())
            return error("%s : no auxpow on block with auxpow version",
                         __func__);

        if (!CheckProofOfWork(block.GetPoWHash(), block.nBits, params))
            return error("%s : non-AUX proof of work failed", __func__);

        return true;
    }

    /* We have auxpow.  Check it.  */

    if (!block.nVersion.IsAuxpow())
        return error("%s : auxpow on block with non-auxpow version", __func__);

    if (!block.auxpow->check(block.GetHash(), block.nVersion.GetChainId(), params))
        return error("%s : AUX POW is not valid", __func__);
    if (!CheckProofOfWork(block.auxpow->getParentBlockPoWHash(), block.nBits, params))
        return error("%s : AUX proof of work failed", __func__);

    return true;
}

CAmount GetNewYorkCoinBlockSubsidy(int nHeight, const Consensus::Params& consensusParams, uint256 prevHash)
{
      int64_t maxSubsidy = 10000 * COIN;
      int64_t minSubsidy = 50 * COIN;
      int64_t nSubsidy = maxSubsidy;

       std::string cseed_str = prevHash.ToString().substr(7,7);
      const char* cseed = cseed_str.c_str();

      long seed = hex2long(cseed);
      int rand = generateMTRandom(seed, 999999);
      int rand1 = 0;
      int rand2 = 0;
      int rand3 = 0;
      int rand4 = 0;
      int rand5 = 0;




       // Start of botched pre-mine
       // The following if/else block is overridden by the subsequent if/else block
       // Leaving this for historical accuracy
       if(consensusParams.nHeightEffective == 0) //Legacy
       {
           if (nHeight == 1) {
            nSubsidy = 97000000 * COIN;
           }
           else if (nHeight < 101) {
            nSubsidy = 1 * COIN;
           }
           // End of botched pre-mine

           if (nHeight < 100000) {
            nSubsidy = (1 + rand) * COIN;
           } else if (nHeight < 200000) {
            cseed_str = prevHash.ToString().substr(7,7);
            cseed = cseed_str.c_str();
            seed = hex2long(cseed);
            rand1 = generateMTRandom(seed, 499999);
            nSubsidy = (1 + rand1) * COIN;
           } else if (nHeight < 300000) {
            cseed_str = prevHash.ToString().substr(6,7);
            cseed = cseed_str.c_str();
            seed = hex2long(cseed);
            rand2 = generateMTRandom(seed, 249999);
            nSubsidy = (1 + rand2) * COIN;
           } else if (nHeight < 400000) {
            cseed_str = prevHash.ToString().substr(7,7);
            cseed = cseed_str.c_str();
            seed = hex2long(cseed);
            rand3 = generateMTRandom(seed, 124999);
            nSubsidy = (1 + rand3) * COIN;
           } else if (nHeight < 500000) {
            cseed_str = prevHash.ToString().substr(7,7);
            cseed = cseed_str.c_str();
            seed = hex2long(cseed);
            rand4 = generateMTRandom(seed, 62499);
            nSubsidy = (1 + rand4) * COIN;
           } else if (nHeight < 600000) {
            cseed_str = prevHash.ToString().substr(6,7);
            cseed = cseed_str.c_str();
            seed = hex2long(cseed);
            rand5 = generateMTRandom(seed, 31249);
            nSubsidy = (1 + rand5) * COIN;
          } else if(nHeight > 4600000) {   	             // 2018-4-14 - Block height 4,234,899
              nSubsidy = maxSubsidy / 2;                   // 250,000 blocks is ~3 months based on data from 2018
           } else if(nHeight > 5000000) {
              nSubsidy = maxSubsidy / 4;
           } else if(nHeight > 5500000) {
              nSubsidy = maxSubsidy / 8;
           } else if(nHeight > 6000000) {
              nSubsidy = maxSubsidy / 16;
           } else if(nHeight > 6500000) {
              nSubsidy = minSubsidy * 2;
           } else if(nHeight > 7000000) {
              nSubsidy = minSubsidy;
           }
       }
       else
       {
             int halvings = nHeight / consensusParams.nSubsidyHalvingInterval;

              if(nHeight < (9 * consensusParams.nSubsidyHalvingInterval)) // < 4,500,000
              {
                 return 10000 * COIN;
              }
              else if (nHeight < (10 * consensusParams.nSubsidyHalvingInterval)) // < 5,000,000
              {
                return 5000 * COIN;
              }
              else if (nHeight < (11 * consensusParams.nSubsidyHalvingInterval)) // < 5,500,000
              {
                return 2500 * COIN;
              }
              else if (nHeight < (12 * consensusParams.nSubsidyHalvingInterval)) //< 6,000,000
              {
                return 1250 * COIN;
              }
              else if (nHeight < (13 * consensusParams.nSubsidyHalvingInterval)) // < 6,500,000
              {
                return 625 * COIN;
              }
              else if (nHeight < (14 * consensusParams.nSubsidyHalvingInterval)) // < 7,000,000
              {
                return 100 * COIN;
              }
              else
              {
                return 50 * COIN;
              }

       }



    return nSubsidy;
}


int64_t GetNewYorkCoinDustFee(const std::vector<CTxOut> &vout, CFeeRate &baseFeeRate) {
    int64_t nFee = 0;

    // To limit dust spam, add base fee for each dust output
    BOOST_FOREACH(const CTxOut& txout, vout)
        // if (txout.IsDust(::minRelayTxFee))
        if (txout.nValue < CENT)
            nFee += baseFeeRate.GetFeePerK();

    return nFee;
}

static const long hextable[] =
{
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,		// 10-19
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,		// 30-39
	-1, -1, -1, -1, -1, -1, -1, -1,  0,  1,
	 2,  3,  4,  5,  6,  7,  8,  9, -1, -1,		// 50-59
	-1, -1, -1, -1, -1, 10, 11, 12, 13, 14,
	15, -1, -1, -1, -1, -1, -1, -1, -1, -1,		// 70-79
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, 10, 11, 12,		// 90-99
	13, 14, 15, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,		// 110-109
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,		// 130-139
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,		// 150-159
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,		// 170-179
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,		// 190-199
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,		// 210-219
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,		// 230-239
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1
};

long hex2long(const char* hexString)
{
	long ret = 0;

	while (*hexString && ret >= 0)
	{
		ret = (ret << 4) | hextable[*hexString++];
	}

	return ret;
}
