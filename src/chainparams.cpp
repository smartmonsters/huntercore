// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"

#include "names/common.h"
#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"

bool CChainParams::IsHistoricBug(const uint256& txid, unsigned nHeight, BugType& type) const
{
    const std::pair<unsigned, uint256> key(nHeight, txid);
    std::map<std::pair<unsigned, uint256>, BugType>::const_iterator mi;

    mi = mapHistoricBugs.find (key);
    if (mi != mapHistoricBugs.end ())
    {
        type = mi->second;
        return true;
    }

    return false;
}

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << ValtypeFromString(std::string(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion.SetGenesisVersion(nVersion);
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = genesis.ComputeMerkleRoot();
    return genesis;
}

/**
 * Build the genesis block.
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char *pszTimestamp =
            "\n"
            "Huntercoin genesis timestamp\n"
            "31/Jan/2014 20:10 GMT\n"
            "Bitcoin block 283440: 0000000000000001795d3c369b0746c0b5d315a6739a7410ada886de5d71ca86\n"
            "Litecoin block 506479: 77c49384e6e8dd322da0ebb32ca6c8f047d515d355e9f22b116430a888fffd38\n"
        ;
    const CScript genesisOutputScript = CScript() << OP_DUP << OP_HASH160 << ParseHex("fe2435b201d25290533bdaacdfe25dc7548b3058") << OP_EQUALVERIFY << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Build genesis block for testnet.  In Huntercoin, it has a changed timestamp
 * and output script.
 */
static CBlock CreateTestnetGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "\nHuntercoin test net\n";
    const CScript genesisOutputScript = CScript() << OP_DUP << OP_HASH160 << ParseHex("7238d2df990b8e333ed28a84a8df8408f6dbcd57") << OP_EQUALVERIFY << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

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
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 2100000;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        /* FIXME: Set once we need the value in main.cpp.  */
        consensus.BIP34Height = -1;
        consensus.BIP34Hash = uint256();
        consensus.powLimit[ALGO_SHA256D] = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimit[ALGO_SCRYPT] = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetSpacing = 60 * NUM_ALGOS;
        consensus.nPowTargetTimespan = consensus.nPowTargetSpacing * 2016;
        consensus.fPowNoRetargeting = false;

        consensus.nAuxpowChainId[ALGO_SHA256D] = 0x0006;
        consensus.nAuxpowChainId[ALGO_SCRYPT] = 0x0002;
        consensus.fStrictChainId = true;

        consensus.rules.reset(new Consensus::MainNetConsensus());

        /** 
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xf9;
        pchMessageStart[1] = 0xbe;
        pchMessageStart[2] = 0xb4;
        pchMessageStart[3] = 0xfe;
        vAlertPubKey = ParseHex("04d55568f5688898159fd01640f6c7ef2e63fef95376e8418244b4c7c4dd57110d8028f4086a092f2586dc09b36359e67e0717a0bec2a483c81aaf252377fc666a");
        nDefaultPort = 8398;
        nMaxTipAge = 24 * 60 * 60;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(1391199780, 1906435634u, 486604799, 1, 85000 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x00000000db7eb7a9e1a06cf995363dcdc4c28e8ae04827a961942657db9a1631"));
        assert(genesis.hashMerkleRoot == uint256S("0xc4ee946ffcb0bffa454782432d530bbeb8562b09594c1fbc8ceccd46ce34a754"));

        /* FIXME: Add DNS seeds.  */
        //vSeeds.push_back(CDNSSeedData("quisquis.de", "nmc.seed.quisquis.de"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,40);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,13); // FIXME: Update.
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,168);
        /* FIXME: Update these below.  */
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();

        /* FIXME: fixed seeds?  */
        //vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (     0, uint256S("00000000db7eb7a9e1a06cf995363dcdc4c28e8ae04827a961942657db9a1631")),
            0, // * UNIX timestamp of last checkpoint block
            0, // * total number of transactions between genesis and last checkpoint
               //   (the tx=... number in the SetBestChain debug.log lines)
            0  // * estimated number of transactions per day after checkpoint
        };
    }

    int DefaultCheckNameDB () const
    {
        return -1;
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.nMajorityEnforceBlockUpgrade = 51;
        consensus.nMajorityRejectBlockOutdated = 75;
        consensus.nMajorityWindow = 100;
        /* FIXME: Set once we need the value in main.cpp.  */
        consensus.BIP34Height = -1;
        consensus.BIP34Hash = uint256();
        consensus.powLimit[ALGO_SHA256D] = uint256S("000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimit[ALGO_SCRYPT] = uint256S("000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetSpacing = 60 * NUM_ALGOS;
        consensus.nPowTargetTimespan = consensus.nPowTargetSpacing * 2016;
        consensus.fPowNoRetargeting = false;

        consensus.nAuxpowChainId[ALGO_SHA256D] = 0x0006;
        consensus.nAuxpowChainId[ALGO_SCRYPT] = 0x0002;
        consensus.fStrictChainId = false;

        consensus.rules.reset(new Consensus::TestNetConsensus());

        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xfe;
        /* FIXME: Update alert key.  */
        vAlertPubKey = ParseHex("04fc9702847840aaf195de8442ebecedf5b095cdbb9bc716bda9110971b28a49e0ead8564ff0db22209e0374782c093bb899692d524e9d6a6956e7c5ecbcd68284");
        nDefaultPort = 18398;
        nMaxTipAge = 0x7fffffff;
        nPruneAfterHeight = 1000;

        genesis = CreateTestnetGenesisBlock(1391193136, 1997599826u, 503382015, 1, 100 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("000000492c361a01ce7558a3bfb198ea3ff2f86f8b0c2e00d26135c53f4acbf7"));
        assert(genesis.hashMerkleRoot == uint256S("28da665eada1b006bb9caf83e7541c6f995e0681debfc2540507bbfdf2d4ac84"));

        vFixedSeeds.clear();
        vSeeds.clear();
        /* FIXME: Testnet seeds?  */
        //vSeeds.push_back(CDNSSeedData("webbtc.com", "dnsseed.test.namecoin.webbtc.com"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,100);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196); // FIXME: Update
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,228);
        /* FIXME: Update these below.  */
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        /* FIXME: fixed seeds?  */
        //vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (     0, uint256S("000000492c361a01ce7558a3bfb198ea3ff2f86f8b0c2e00d26135c53f4acbf7")),
            0,
            0,
            0
        };

        assert(mapHistoricBugs.empty());
    }

    int DefaultCheckNameDB () const
    {
        return -1;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
// FIXME: Update
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.BIP34Height = -1; // BIP34 has not necessarily activated on regtest
        consensus.BIP34Hash = uint256();
        consensus.powLimit[ALGO_SHA256D] = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimit[ALGO_SCRYPT] = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetSpacing = 60 * NUM_ALGOS;
        consensus.nPowTargetTimespan = consensus.nPowTargetSpacing * 2016;
        consensus.fPowNoRetargeting = true;

        consensus.nAuxpowChainId[ALGO_SHA256D] = 0x0006;
        consensus.nAuxpowChainId[ALGO_SCRYPT] = 0x0002;
        consensus.fStrictChainId = true;

        consensus.rules.reset(new Consensus::RegTestConsensus());

        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xda;
        nMaxTipAge = 24 * 60 * 60;
        nDefaultPort = 18445;
        nPruneAfterHeight = 1000;

        genesis = CreateTestnetGenesisBlock(1296688602, 2, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        // FIXME: Enable once parameters are set.
        //assert(consensus.hashGenesisBlock == uint256S("0x0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206"));
        //assert(genesis.hashMerkleRoot == uint256S("0x4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"));

        vFixedSeeds.clear(); //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData){
            boost::assign::map_list_of
            ( 0, uint256S("5287b3809b71433729402429b7d909a853cfac5ed40f09117b242c275e6b2d63")),
            0,
            0,
            0
        };
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        assert(mapHistoricBugs.empty());
    }

    int DefaultCheckNameDB () const
    {
        return 0;
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
            return mainParams;
    else if (chain == CBaseChainParams::TESTNET)
            return testNetParams;
    else if (chain == CBaseChainParams::REGTEST)
            return regTestParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}
