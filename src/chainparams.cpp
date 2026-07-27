// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2015-2025 The Zcash developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php .

#include "chainparams.h"
#include "consensus/merkle.h"
#include "key_io.h"
#include "main.h"
#include "crypto/equihash.h"

#include "tinyformat.h"
#include "util/system.h"
#include "util/strencodings.h"

#include <assert.h>
#include <optional>
#include <variant>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, const uint256& nNonce, const std::vector<unsigned char>& nSolution, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    // To create a genesis block for a new chain which is Overwintered:
    //   txNew.nVersion = OVERWINTER_TX_VERSION
    //   txNew.fOverwintered = true
    //   txNew.nVersionGroupId = OVERWINTER_VERSION_GROUP_ID
    //   txNew.nExpiryHeight = <default value>
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 520617983 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nSolution = nSolution;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database (and is in any case of zero value).
 *
 * >>> from hashlib import blake2s
 * >>> 'Zcash' + blake2s(b'The Economist 2016-10-29 Known unknown: Another crypto-currency is born. BTC#436254 0000000000000000044f321997f336d2908cf8c8d6893e88dbf067e2d949487d ETH#2521903 483039a6b6bd8bd05f0584f9a078d075e454925eb71c1f13eaff59b405a721bb DJIA close on 27 Oct 2016: 18,169.68').hexdigest()
 *
 * CBlock(hash=00040fe8, ver=4, hashPrevBlock=00000000000000, hashMerkleRoot=c4eaa5, nTime=1477641360, nBits=1f07ffff, nNonce=4695, vtx=1)
 *   CTransaction(hash=c4eaa5, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff071f0104455a6361736830623963346565663862376363343137656535303031653335303039383462366665613335363833613763616331343161303433633432303634383335643334)
 *     CTxOut(nValue=0.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: c4eaa5
 */
static CBlock CreateGenesisBlock(uint32_t nTime, const uint256& nNonce, const std::vector<unsigned char>& nSolution, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Zcash0b9c4eef8b7cc417ee5001e3500984b6fea35683a7cac141a043c42064835d34";
    const CScript genesisOutputScript = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nSolution, nBits, nVersion, genesisReward);
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

const arith_uint256 maxUint = UintToArith256(uint256S("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"));

class CMainParams : public CChainParams {
public:
    CMainParams() {
        keyConstants.strNetworkID = "main";
        strCurrencyUnits = "SPEC";
        keyConstants.bip44CoinType = 19117; // Spectre (SLIP-0044: unregistered)
        consensus.fCoinbaseMustBeShielded = true;
        consensus.nSubsidySlowStartInterval = 20000;
        consensus.nPreBlossomSubsidyHalvingInterval = Consensus::PRE_BLOSSOM_HALVING_INTERVAL;
        consensus.nPostBlossomSubsidyHalvingInterval = POST_BLOSSOM_HALVING_INTERVAL(Consensus::PRE_BLOSSOM_HALVING_INTERVAL);
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 4000;
        const size_t N = 200, K = 9;
        static_assert(equihash_parameters_acceptable(N, K));
        consensus.nEquihashN = N;
        consensus.nEquihashK = K;
        consensus.powLimit = uint256S("0007ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowAveragingWindow = 17;
        assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nPowAveragingWindow);
        consensus.nPowMaxAdjustDown = 32; // 32% adjustment down
        consensus.nPowMaxAdjustUp = 16; // 16% adjustment up
        consensus.nPreBlossomPowTargetSpacing = Consensus::PRE_BLOSSOM_POW_TARGET_SPACING;
        consensus.nPostBlossomPowTargetSpacing = Consensus::POST_BLOSSOM_POW_TARGET_SPACING;
        consensus.nPowAllowMinDifficultyBlocksAfterHeight = std::nullopt;
        consensus.fPowNoRetargeting = false;
        consensus.vUpgrades[Consensus::BASE_SPROUT].nProtocolVersion = 170002;
        consensus.vUpgrades[Consensus::BASE_SPROUT].nActivationHeight =
            Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nProtocolVersion = 170002;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].nProtocolVersion = 170005;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].nActivationHeight = 347500;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].hashActivationBlock =
            uint256S("0000000003761c0d0c3974b54bdb425613bbb1eaadd6e70b764de82f195ea243");
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].nProtocolVersion = 170007;
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].nActivationHeight = 419200;
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].hashActivationBlock =
            uint256S("00000000025a57200d898ac7f21e26bf29028bbe96ec46e05b2c17cc9db9e4f3");
        consensus.vUpgrades[Consensus::UPGRADE_BLOSSOM].nProtocolVersion = 170009;
        consensus.vUpgrades[Consensus::UPGRADE_BLOSSOM].nActivationHeight = 653600;
        consensus.vUpgrades[Consensus::UPGRADE_BLOSSOM].hashActivationBlock =
            uint256S("00000000020bebb33c1b34b67a982a328ab212a206dacbe561a7cc94aab3e9bb");
        consensus.vUpgrades[Consensus::UPGRADE_HEARTWOOD].nProtocolVersion = 170011;
        consensus.vUpgrades[Consensus::UPGRADE_HEARTWOOD].nActivationHeight = 903000;
        consensus.vUpgrades[Consensus::UPGRADE_HEARTWOOD].hashActivationBlock =
            uint256S("0000000000aad1c8698964a93c35ecf8b4d05e848de9e2fe7606067139be5643");
        consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nProtocolVersion = 170013;
        consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nActivationHeight = 1046400;
        consensus.vUpgrades[Consensus::UPGRADE_CANOPY].hashActivationBlock =
            uint256S("00000000002038016f976744c369dce7419fca30e7171dfac703af5e5f7ad1d4");
        consensus.vUpgrades[Consensus::UPGRADE_NU5].nProtocolVersion = 170100;
        consensus.vUpgrades[Consensus::UPGRADE_NU5].nActivationHeight = 1687104;
        consensus.vUpgrades[Consensus::UPGRADE_NU5].hashActivationBlock =
            uint256S("0000000000d723156d9b65ffcf4984da7a19675ed7e2f06d9e5d5188af087bf8");
        consensus.vUpgrades[Consensus::UPGRADE_NU6].nProtocolVersion = 170120;
        consensus.vUpgrades[Consensus::UPGRADE_NU6].nActivationHeight = 2726400;
        consensus.vUpgrades[Consensus::UPGRADE_NU6_1].nProtocolVersion = 170140;
        consensus.vUpgrades[Consensus::UPGRADE_NU6_1].nActivationHeight = 3146400;
        consensus.nTemporaryOrchardDisablingSoftForkHeight = 3363426;
        consensus.vUpgrades[Consensus::UPGRADE_NU6_2].nProtocolVersion = 170150;
        consensus.vUpgrades[Consensus::UPGRADE_NU6_2].nActivationHeight = 3364600;
        consensus.vUpgrades[Consensus::UPGRADE_ZFUTURE].nProtocolVersion = 0x7FFFFFFF;
        consensus.vUpgrades[Consensus::UPGRADE_ZFUTURE].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;

        consensus.nFundingPeriodLength = consensus.nPostBlossomSubsidyHalvingInterval / 48;

        // guarantees the first 2 characters, when base58 encoded, are "s1"
        keyConstants.base58Prefixes[PUBKEY_ADDRESS]     = {0x1C,0x27};
        // guarantees the first 2 characters, when base58 encoded, are "s3"
        keyConstants.base58Prefixes[SCRIPT_ADDRESS]     = {0x1C,0x2C};
        // the first character, when base58 encoded, is "5" or "K" or "L" (as in Bitcoin)
        keyConstants.base58Prefixes[SECRET_KEY]         = {0x80};
        // do not rely on these BIP32 prefixes; they are not specified and may change
        keyConstants.base58Prefixes[EXT_PUBLIC_KEY]     = {0x04,0x88,0xB2,0x1E};
        keyConstants.base58Prefixes[EXT_SECRET_KEY]     = {0x04,0x88,0xAD,0xE4};
        // guarantees the first 2 characters, when base58 encoded, are "sc"
        keyConstants.base58Prefixes[ZCPAYMENT_ADDRESS]  = {0x16,0xA1};
        // guarantees the first 4 characters, when base58 encoded, are "ZiVK"
        keyConstants.base58Prefixes[ZCVIEWING_KEY]      = {0xA8,0xAB,0xD3};
        // guarantees the first 2 characters, when base58 encoded, are "SK"
        keyConstants.base58Prefixes[ZCSPENDING_KEY]     = {0xAB,0x36};

        keyConstants.bech32HRPs[SAPLING_PAYMENT_ADDRESS]      = "spec";
        keyConstants.bech32HRPs[SAPLING_FULL_VIEWING_KEY]     = "specviews";
        keyConstants.bech32HRPs[SAPLING_INCOMING_VIEWING_KEY] = "specivks";
        keyConstants.bech32HRPs[SAPLING_EXTENDED_SPEND_KEY]   = "secret-extended-key-spec";
        keyConstants.bech32HRPs[SAPLING_EXTENDED_FVK]         = "sexviews";

        keyConstants.bech32mHRPs[TEX_ADDRESS]                 = "stex";
        {
            auto canopyActivation = consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nActivationHeight;
            auto nu6Activation = consensus.vUpgrades[Consensus::UPGRADE_NU6].nActivationHeight;
            auto nu6_1Activation = consensus.vUpgrades[Consensus::UPGRADE_NU6_1].nActivationHeight;

            // ZIP 214 Revision 0
            std::vector<std::string> bp_addresses = {
                "s3CcypyCQoHXMU6WzQs2FD1jsHCSSZkM3TR",
                "s3DfqG82NC8jso3KWoQTFzertZ638YWgQxC",
                "s3538RehBY9Qfh1Jtu9dgHxEAG31eaLneEA",
                "s3H6zi46PkJEH3UC4Xd5ifzM4KKxo8eYsJB",
                "s2yyXVKUr6YHizFqU6LTrjuejsDsEJkhFiA",
                "s36Be6DwWTTsNXEiKZp48EXBBDUVDrpu7tc",
                "s34AuWHGD7qpQ4Bsuc8n9KeYFNve6NMXMSV",
                "s3719SUmT5CfMpbpREMVmHLQ7F8dSDv4FKC",
                "s39MjmFiTq8j8267o2sZmh497QQJvfo6J6B",
                "s3AvFFvcS76ANUseqnVAJ8htbn6ghtv6sGF",
                "s3EgsC9GY8ZjfL9DHszSZuUvY5dMG1P4CTm",
                "s3Fb2rCUMxzMuzKkrAsndWgh57qWxvwwe2J",
                "s3BELw18NpvVLAajagGbZGBVMJfhStcobrR",
                "s37afJfz9x9G2GQFKpreERgTv7qSQrwdixA",
                "s2xmb2JbWaJZyuEoe486uTZeg8P5WpySQ5T",
                "s3Dp8nTwKRUzHD5PxMsvBCT7d9k3ub7LPx5",
                "s3Ch1XQgzMypJX8S9yi2TavDnaCNRCBiVde",
                "s2ywq86TgdJ4fSnLNovs3xMhdFpL12iWML1",
                "s3HDaQDBKwgc2Hw4Biou2yqhHnxbSFEyJYE",
                "s3JXrzGTVCtsRvocSwBJTPoe8nDupSexYKq",
                "s3FaeEHVgM4VDrD3QCJqkjvDNxF92XgoEDm",
                "s35PjBvdp9g76UXZ9MLwpcrRJJRpfQgWWyF",
                "s36q5ei9QWwusi4zTM8cdsWRzd9nVb2VG6M",
                "s3MjgD5GMcmZZA3AWJ7jvCSjEKm78CRSDcK",
                "s33Zdry7GErKJvrhT6W4fJHtNP787b388GN",
                "s31EuPHPWL3ZZAFHB2UTPx8asH2GmrNECfT",
                "s328nHhLE5tVGiNLfRewbHbpSKoqYcGTtS5",
                "s3HcC7Qgmzv198Nj2xrLDch8oqQTt2Fk9mk",
                "s2zYgGx28HCwmdKX6QGNcM8QZuqDtijFoXM",
                "s36d84TxfS2WiyWX3hzW3nk7UQaXRU6x1HX",
                "s3BvY7tkPmP5FWBxCwqrXw2zcvDKdRnxoj2",
                "s36wd2jUhnd64VVs4PwKxwB8ztjmQPEJCMm",
                "s3LRXJgR1WSj7GqyfoDGSxPJvbWP67q28Tr",
                "s3MLDQSQesPizrGLHs4iiQEabzAvYcjigVn",
                "s33twWoUF3tvVNh8w5tSZBY7JySiE1Sxm9F",
                "s2yeTc58JNApqFa7Dmi2oPfWH6zPVV89GKm",
                "s3DDwJxQkoKgBvMoTPxtRhnWEtPVYEakSHj",
                "s3G1ckKQtnLgNvpx5HwMvoPRzW1hSztw6bj",
                "s3HhGjXB4xbwNT4pji5EmCxkRkmrUj4gYSh",
                "s34ciTXUF23U8EvVP1e2eYqTVwnFXZxXdmY",
                "s3LBmfzLSqABTF6yGpsjLXxKRiBPp1PMxqB",
                "s2y5YDLHSV6t2VzM1AmZLVyvXAH8Mu3uqib",
                "s3GhkEN4kYvtPUuHPHZ3iJweDbPfGDVWk5k",
                "s3J3AmiQCEzxwT7VjxHKNLHZauUpvkgvHmj",
                "s2zgDBvGrqTwPhgqsZQZCszjVgpDw7MDKqS",
                "s32nZ7pbeH7WqeyGwTJ4z7zNdQrjMti2ERx",
                "s2xwXEo6bZoMAwQPeD88P3VqKACHJb4osoQ",
                "s36iBtJC52t7vbckimzp7795gxEaL7ysTiK",
                "s3GTvGY8eaQ3mdNFbGR5B1EuoqQycmBzyyF",
                "s3HQWNJtRn2cr6WhmTfvxkbTrxCHuraonjX",
                "s3A91CDeoZuTKiGXpRsw4YzyWXrX28AA695",
            };

            // ZF and MG each use a single address repeated 48 times,
            // once for each funding period.
            std::vector<std::string> zf_addresses(48, "s31623Mb8RChRk33xBs8JPt8qXLAy3vxehA");
            std::vector<std::string> mg_addresses(48, "s373VEaDPV2Pe8Z89fnyDf7idNQyyCZKrV1");

            consensus.AddZIP207FundingStream(
                keyConstants,
                Consensus::FS_ZIP214_BP,
                canopyActivation,
                nu6Activation,
                bp_addresses);
            consensus.AddZIP207FundingStream(
                keyConstants,
                Consensus::FS_ZIP214_ZF,
                canopyActivation,
                nu6Activation,
                zf_addresses);
            consensus.AddZIP207FundingStream(
                keyConstants,
                Consensus::FS_ZIP214_MG,
                canopyActivation,
                nu6Activation,
                mg_addresses);

            // ZIP 214 Revision 1
            // FPF uses a single address repeated 12 times, once for each funding period.
            std::vector<std::string> fpf_addresses(12, "s38s8KktTicvXF6Ug9LV5oqyfJpDdSMgUdV");

            consensus.AddZIP207FundingStream(
                keyConstants,
                Consensus::FS_FPF_ZCG,
                nu6Activation,
                nu6_1Activation,
                fpf_addresses);
            consensus.AddZIP207LockboxStream(
                keyConstants,
                Consensus::FS_DEFERRED,
                nu6Activation,
                nu6_1Activation);

            // ZIP 214 Revision 2
            // FPF uses a single address repeated 36 times, once for each funding period.
            std::vector<std::string> fpf_addresses_h3(36, "s38s8KktTicvXF6Ug9LV5oqyfJpDdSMgUdV");
            consensus.AddZIP207FundingStream(
                keyConstants,
                Consensus::FS_FPF_ZCG_H3,
                nu6_1Activation,
                4406400,
                fpf_addresses_h3);
            consensus.AddZIP207LockboxStream(
                keyConstants,
                Consensus::FS_CCF_H3,
                nu6_1Activation,
                4406400);

            // ZIP 271
            // For convenience of distribution, we split the lockbox contents into 10 equal chunks.
            std::string nu6_1_kho_address = "s3FgAJKvs1KK6U4TRYmX8ELq3FHVHdojyZt";
            static const CAmount nu6_1_disbursement_amount = 78750 * COIN;
            static const CAmount nu6_1_chunk_amount = 7875 * COIN;
            static constexpr auto nu6_1_chunks = {
                Consensus::LD_ZIP271_NU6_1_CHUNK_1,
                Consensus::LD_ZIP271_NU6_1_CHUNK_2,
                Consensus::LD_ZIP271_NU6_1_CHUNK_3,
                Consensus::LD_ZIP271_NU6_1_CHUNK_4,
                Consensus::LD_ZIP271_NU6_1_CHUNK_5,
                Consensus::LD_ZIP271_NU6_1_CHUNK_6,
                Consensus::LD_ZIP271_NU6_1_CHUNK_7,
                Consensus::LD_ZIP271_NU6_1_CHUNK_8,
                Consensus::LD_ZIP271_NU6_1_CHUNK_9,
                Consensus::LD_ZIP271_NU6_1_CHUNK_10,
            };
            static_assert(nu6_1_chunk_amount * nu6_1_chunks.size() == nu6_1_disbursement_amount);
            for (auto idx : nu6_1_chunks) {
                consensus.AddZIP271LockboxDisbursement(
                    keyConstants,
                    idx,
                    Consensus::UPGRADE_NU6_1,
                    nu6_1_chunk_amount,
                    nu6_1_kho_address);
            }
        }

        // The best chain should have at least this much work (updated as chain grows).
        consensus.nMinimumChainWork = uint256S("0x00");

        // Spectre mainnet P2P magic ("SPEC")
        pchMessageStart[0] = 0x53;
        pchMessageStart[1] = 0x70;
        pchMessageStart[2] = 0x45;
        pchMessageStart[3] = 0x43;
        nDefaultPort = 8733;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(
            1477641360,
            uint256S("0x0000000000000000000000000000000000000000000000000000000000001257"),
            ParseHex("000a889f00854b8665cd555f4656f68179d31ccadc1b1f7fb0952726313b16941da348284d67add4686121d4e3d930160c1348d8191c25f12b267a6a9c131b5031cbf8af1f79c9d513076a216ec87ed045fa966e01214ed83ca02dc1797270a454720d3206ac7d931a0a680c5c5e099057592570ca9bdf6058343958b31901fce1a15a4f38fd347750912e14004c73dfe588b903b6c03166582eeaf30529b14072a7b3079e3a684601b9b3024054201f7440b0ee9eb1a7120ff43f713735494aa27b1f8bab60d7f398bca14f6abb2adbf29b04099121438a7974b078a11635b594e9170f1086140b4173822dd697894483e1c6b4e8b8dcd5cb12ca4903bc61e108871d4d915a9093c18ac9b02b6716ce1013ca2c1174e319c1a570215bc9ab5f7564765f7be20524dc3fdf8aa356fd94d445e05ab165ad8bb4a0db096c097618c81098f91443c719416d39837af6de85015dca0de89462b1d8386758b2cf8a99e00953b308032ae44c35e05eb71842922eb69797f68813b59caf266cb6c213569ae3280505421a7e3a0a37fdf8e2ea354fc5422816655394a9454bac542a9298f176e211020d63dee6852c40de02267e2fc9d5e1ff2ad9309506f02a1a71a0501b16d0d36f70cdfd8de78116c0c506ee0b8ddfdeb561acadf31746b5a9dd32c21930884397fb1682164cb565cc14e089d66635a32618f7eb05fe05082b8a3fae620571660a6b89886eac53dec109d7cbb6930ca698a168f301a950be152da1be2b9e07516995e20baceebecb5579d7cdbc16d09f3a50cb3c7dffe33f26686d4ff3f8946ee6475e98cf7b3cf9062b6966e838f865ff3de5fb064a37a21da7bb8dfd2501a29e184f207caaba364f36f2329a77515dcb710e29ffbf73e2bbd773fab1f9a6b005567affff605c132e4e4dd69f36bd201005458cfbd2c658701eb2a700251cefd886b1e674ae816d3f719bac64be649c172ba27a4fd55947d95d53ba4cbc73de97b8af5ed4840b659370c556e7376457f51e5ebb66018849923db82c1c9a819f173cccdb8f3324b239609a300018d0fb094adf5bd7cbb3834c69e6d0b3798065c525b20f040e965e1a161af78ff7561cd874f5f1b75aa0bc77f720589e1b810f831eac5073e6dd46d00a2793f70f7427f0f798f2f53a67e615e65d356e66fe40609a958a05edb4c175bcc383ea0530e67ddbe479a898943c6e3074c6fcc252d6014de3a3d292b03f0d88d312fe221be7be7e3c59d07fa0f2f4029e364f1f355c5d01fa53770d0cd76d82bf7e60f6903bc1beb772e6fde4a70be51d9c7e03c8d6d8dfb361a234ba47c470fe630820bbd920715621b9fbedb49fcee165ead0875e6c2b1af16f50b5d6140cc981122fcbcf7c5a4e3772b3661b628e08380abc545957e59f634705b1bbde2f0b4e055a5ec5676d859be77e20962b645e051a880fddb0180b4555789e1f9344a436a84dc5579e2553f1e5fb0a599c137be36cabbed0319831fea3fddf94ddc7971e4bcf02cdc93294a9aab3e3b13e3b058235b4f4ec06ba4ceaa49d675b4ba80716f3bc6976b1fbf9c8bf1f3e3a4dc1cd83ef9cf816667fb94f1e923ff63fef072e6a19321e4812f96cb0ffa864da50ad74deb76917a336f31dce03ed5f0303aad5e6a83634f9fcc371096f8288b8f02ddded5ff1bb9d49331e4a84dbe1543164438fde9ad71dab024779dcdde0b6602b5ae0a6265c14b94edd83b37403f4b78fcd2ed555b596402c28ee81d87a909c4e8722b30c71ecdd861b05f61f8b1231795c76adba2fdefa451b283a5d527955b9f3de1b9828e7b2e74123dd47062ddcc09b05e7fa13cb2212a6fdbc65d7e852cec463ec6fd929f5b8483cf3052113b13dac91b69f49d1b7d1aec01c4a68e41ce157"),
            0x1f07ffff, 4, 0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x00040fe8ec8471911baa1db1266ea15dd06b4a8a5c453883c000b031973dce08"));
        assert(genesis.hashMerkleRoot == uint256S("0xc4eaa58879081de3c24a7b117ed2b28300e7ec4c4c1dff1d3f1268b7857a4ddb"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // Spectre seed nodes: add DNS seeds here before mainnet launch.

        vFixedSeeds.clear();

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (0, consensus.hashGenesisBlock),
            0,
            0,
            0
        };

        // Fresh Spectre chain: no legacy value-pool or supply checkpoints.
        nSproutValuePoolCheckpointHeight = 0;
        nSproutValuePoolCheckpointBalance = 0;
        fZIP209Enabled = true;
        hashSproutValuePoolCheckpointBlock.SetNull();
        nChainSupplyCheckpointHeight = 0;
        nChainSupplyCheckpointTotalSupply = 0;
        nChainSupplyCheckpointTransparentValue = 0;
        nChainSupplyCheckpointSproutValue = 0;
        nChainSupplyCheckpointSaplingValue = 0;
        nChainSupplyCheckpointOrchardValue = 0;
        nChainSupplyCheckpointLockboxValue = 0;
        hashChainSupplyCheckpointBlock.SetNull();

        // Placeholder P2SH addresses — replace with real 2-of-3 multisig before mainnet.
        // Generated by contrib/devtools/gen_spectre_addresses.py
        vFoundersRewardAddress = {
            "s33b9CNWtGSjg1pRkcnkyvXQoWytwm16sJx",
            "s3E7nj6aVSqcwxgnyR6p3un44kcvMWkSqMD",
            "s2zvGBkdc5Me2jaqGWReteskpRVrXc4U4qE",
            "s38X96nt6bLnA3Kc5ZhRMm8cTaHMcxgwxjE",
            "s3GTZBH5BLpND2DdRGoRfYFehHZxzsidSsp",
            "s383K1RB7jEEJSfDYsHYHyN6wir4M3odKuq",
            "s39m2si1qEz8EgWXVAhDp2tihfmb4h2CLKt",
            "s39bBzQx672rf2jtwgsXfq5uUMaKfVXcA73",
            "s3FjSmBoU1Babh4ZyWRbtVyWiv5CMrnajdp",
            "s34AWRysMWLao4df21epqfdzz1XjZVoZaMM",
            "s32HDQKTsRZeMPUcNHeLDrDM6f73Ebi3AuX",
            "s36QaxmYfybqfMXHNUHoMPxLKu4N9nWLEib",
            "s3HYf9Hzxvvdvz8EaujsKtwbrtZdKmGYXGN",
            "s3C1J9g9HhhzjE59hnrh8Fe4dS3AtXFj3Yb",
            "s3MYZtww5PRYKkjcQGPKzv74Z6mbvPAbJou",
            "s3HC2g8mb2g1F9keGLJzghvm6fdQ2nZ8ysf",
            "s31cmyPGnDMumAgLjtr9sNwJfdmoRGLfShV",
            "s3HxJPXrwX5fdTPq8GiE3NVwA3vqbYwZ6iK",
            "s3KdpNDvScdFiEvUKvouYopxY5cTpmK4D3W",
            "s3JE7PPi7pxddZqSLKtNLCai92G294NK8EB",
            "s3BD4Z9Y74udx8ej3jHbegkFKWcmCHQkhm7",
            "s32pqv86yiGphhhVEoHpfEinzsGYjSLEx2M",
            "s31hi9ZKNUPcUDuVAuMSJR5PD8UtvKjdnNM",
            "s31GfiJbYd5a7ctqEkFvBVnrNvnKxnJPU79",
            "s35UBSP1Cc9H7Sh8nhuVUEmMyH2jUSXrogt",
            "s31RQVQQ9jzWhNYQdDmFPJd4zofX74vxxpz",
            "s3MgUkXvLsg2EfcKDLzjBZKbFAQ78ZP9vpo",
            "s3HEbFUKrS6ctDz3Hi8JYK3WUCMW4kiXqxx",
            "s32XxAwgKEAoQAAYauar19xCNwEcEbfm4QZ",
            "s3Hiot2r24bBFQZEJ4vQTE6VqE5QqPaKkXh",
            "s3ESHaM23BRW39NnjRUX6sBr1t829jPRgMA",
            "s3Gp4XBPU51AYaGqnZhWjPDqvu4AbMmf6UA",
            "s3KjChM5w9KjRWyhf61a3GDY6BsVF6zonQg",
            "s3JQ7wqruRMHMHjVyNttzTiJhisxXYcvxxk",
            "s36556QmL63RxHMwoEwDAGRsdb6ZyUZFcyE",
            "s32bRYPbKUxRtA9jgMmeFBBvwQjuYsTXDiZ",
            "s3DiJUNWnCFxRYzfXy4vta1bmy4BTVyVpij",
            "s2xqMDmT7aRMuowgenqaBwqbuukezaWNrvN",
            "s37M62JvAz7ztxGc6iJjmYmQwkCKLjmyEJz",
            "s3J1Vghzed9qrBhW5ssYg56UC7wf9bY5C6f",
            "s31naw3Gff23e1LiUAeysTHiZaEmTMMkMc5",
            "s3C9ChvKJooL7PMsy1WNrCKpJuyW9Jmt48F",
            "s2yGPcyiwujK8Efxy8MUhEKjrTnG3Pu2Wf5",
            "s36fYx2GpRwgudKuK2rWMUjGs9AqxnxyMWb",
            "s3GUGRcVLNiyw8cMrFY5Tit9PXU2f7Gpq4M",
            "s34ed2Xjo6kPZQRHrb2nR5N54QHkvySEgYc",
            "s36Xvj6wiThhdoxo7bBhE7FoiinqKviqcNx",
            "s2xZP4DHtjL4wGNusZVVJSRjjus1p8EyKtk",
        };

        assert(vFoundersRewardAddress.size() <= consensus.GetLastFoundersRewardBlockHeight(0));
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        keyConstants.strNetworkID = "test";
        strCurrencyUnits = "TSPEC";
        keyConstants.bip44CoinType = 19118; // Spectre testnet
        consensus.fCoinbaseMustBeShielded = true;
        consensus.nSubsidySlowStartInterval = 20000;
        consensus.nPreBlossomSubsidyHalvingInterval = Consensus::PRE_BLOSSOM_HALVING_INTERVAL;
        consensus.nPostBlossomSubsidyHalvingInterval = POST_BLOSSOM_HALVING_INTERVAL(Consensus::PRE_BLOSSOM_HALVING_INTERVAL);
        consensus.nMajorityEnforceBlockUpgrade = 51;
        consensus.nMajorityRejectBlockOutdated = 75;
        consensus.nMajorityWindow = 400;
        const size_t N = 200, K = 9;
        static_assert(equihash_parameters_acceptable(N, K));
        consensus.nEquihashN = N;
        consensus.nEquihashK = K;
        consensus.powLimit = uint256S("07ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowAveragingWindow = 17;
        assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nPowAveragingWindow);
        consensus.nPowMaxAdjustDown = 32; // 32% adjustment down
        consensus.nPowMaxAdjustUp = 16; // 16% adjustment up
        consensus.nPreBlossomPowTargetSpacing = Consensus::PRE_BLOSSOM_POW_TARGET_SPACING;
        consensus.nPostBlossomPowTargetSpacing = Consensus::POST_BLOSSOM_POW_TARGET_SPACING;
        consensus.nPowAllowMinDifficultyBlocksAfterHeight = 299187;
        consensus.fPowNoRetargeting = false;
        consensus.vUpgrades[Consensus::BASE_SPROUT].nProtocolVersion = 170002;
        consensus.vUpgrades[Consensus::BASE_SPROUT].nActivationHeight =
            Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nProtocolVersion = 170002;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].nProtocolVersion = 170003;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].nActivationHeight = 207500;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].hashActivationBlock =
            uint256S("0000257c4331b098045023fcfbfa2474681f4564ab483f84e4e1ad078e4acf44");
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].nProtocolVersion = 170007;
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].nActivationHeight = 280000;
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].hashActivationBlock =
            uint256S("000420e7fcc3a49d729479fb0b560dd7b8617b178a08e9e389620a9d1dd6361a");
        consensus.vUpgrades[Consensus::UPGRADE_BLOSSOM].nProtocolVersion = 170008;
        consensus.vUpgrades[Consensus::UPGRADE_BLOSSOM].nActivationHeight = 584000;
        consensus.vUpgrades[Consensus::UPGRADE_BLOSSOM].hashActivationBlock =
            uint256S("00367515ef2e781b8c9358b443b6329572599edd02c59e8af67db9785122f298");
        consensus.vUpgrades[Consensus::UPGRADE_HEARTWOOD].nProtocolVersion = 170010;
        consensus.vUpgrades[Consensus::UPGRADE_HEARTWOOD].nActivationHeight = 903800;
        consensus.vUpgrades[Consensus::UPGRADE_HEARTWOOD].hashActivationBlock =
            uint256S("05688d8a0e9ff7c04f6f05e6d695dc5ab43b9c4803342d77ae360b2b27d2468e");
        consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nProtocolVersion = 170012;
        consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nActivationHeight = 1028500;
        consensus.vUpgrades[Consensus::UPGRADE_CANOPY].hashActivationBlock =
            uint256S("01a4d7c6aada30c87762c1bf33fff5df7266b1fd7616bfdb5227fa59bd79e7a2");
        consensus.vUpgrades[Consensus::UPGRADE_NU5].nProtocolVersion = 170050;
        consensus.vUpgrades[Consensus::UPGRADE_NU5].nActivationHeight = 1842420;
        consensus.vUpgrades[Consensus::UPGRADE_NU5].hashActivationBlock =
            uint256S("0006d75c60b3093d1b671ff7da11c99ea535df9927c02e6ed9eb898605eb7381");
        consensus.vUpgrades[Consensus::UPGRADE_NU6].nProtocolVersion = 170110;
        consensus.vUpgrades[Consensus::UPGRADE_NU6].nActivationHeight = 2976000;
        consensus.vUpgrades[Consensus::UPGRADE_NU6_1].nProtocolVersion = 170130;
        consensus.vUpgrades[Consensus::UPGRADE_NU6_1].nActivationHeight = 3536500;
        consensus.nTemporaryOrchardDisablingSoftForkHeight = 4048500;
        consensus.vUpgrades[Consensus::UPGRADE_NU6_2].nProtocolVersion = 170150;
        consensus.vUpgrades[Consensus::UPGRADE_NU6_2].nActivationHeight = 4052000;
        consensus.vUpgrades[Consensus::UPGRADE_ZFUTURE].nProtocolVersion = 0x7FFFFFFF;
        consensus.vUpgrades[Consensus::UPGRADE_ZFUTURE].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;

        consensus.nFundingPeriodLength = consensus.nPostBlossomSubsidyHalvingInterval / 48;

        // guarantees the first 2 characters, when base58 encoded, are "tm"
        keyConstants.base58Prefixes[PUBKEY_ADDRESS]     = {0x1D,0x25};
        // guarantees the first 2 characters, when base58 encoded, are "t2"
        keyConstants.base58Prefixes[SCRIPT_ADDRESS]     = {0x1C,0xBA};
        // the first character, when base58 encoded, is "9" or "c" (as in Bitcoin)
        keyConstants.base58Prefixes[SECRET_KEY]         = {0xEF};
        // do not rely on these BIP32 prefixes; they are not specified and may change
        keyConstants.base58Prefixes[EXT_PUBLIC_KEY]     = {0x04,0x35,0x87,0xCF};
        keyConstants.base58Prefixes[EXT_SECRET_KEY]     = {0x04,0x35,0x83,0x94};
        // guarantees the first 2 characters, when base58 encoded, are "zt"
        keyConstants.base58Prefixes[ZCPAYMENT_ADDRESS]  = {0x16,0xB6};
        // guarantees the first 4 characters, when base58 encoded, are "ZiVt"
        keyConstants.base58Prefixes[ZCVIEWING_KEY]      = {0xA8,0xAC,0x0C};
        // guarantees the first 2 characters, when base58 encoded, are "ST"
        keyConstants.base58Prefixes[ZCSPENDING_KEY]     = {0xAC,0x08};

        keyConstants.bech32HRPs[SAPLING_PAYMENT_ADDRESS]      = "tspec";
        keyConstants.bech32HRPs[SAPLING_FULL_VIEWING_KEY]     = "tspecviews";
        keyConstants.bech32HRPs[SAPLING_INCOMING_VIEWING_KEY] = "tspecivks";
        keyConstants.bech32HRPs[SAPLING_EXTENDED_SPEND_KEY]   = "secret-extended-key-tspec";
        keyConstants.bech32HRPs[SAPLING_EXTENDED_FVK]         = "txspecviews";

        keyConstants.bech32mHRPs[TEX_ADDRESS]                 = "tstex";

        // Testnet funding streams
        {
            auto canopyActivation = consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nActivationHeight;
            auto nu6Activation = consensus.vUpgrades[Consensus::UPGRADE_NU6].nActivationHeight;
            auto nu6_1Activation = consensus.vUpgrades[Consensus::UPGRADE_NU6_1].nActivationHeight;

            // ZIP 214 Revision 0
            std::vector<std::string> bp_addresses = {
                "t2MfEL68HKC3ZPvXyuPAdyPE16Eqqy2FYMS",
                "t2PgjehvuZonKaXKM6BcEiWvKPJLpJaNm2v",
                "t2FRGg599AgQSrn53shxK2TRNVDrYw5uMSH",
                "t2Ss7iWEbuKcnesGuuJ67Jm8xPZymKMZt8b",
                "t2Si2nkrf35KJTyUiKHYagSS9hxuYZA9BWE",
                "t2RhUNoBiE6NuHXX9B97S379JqRiECGiJD2",
                "t2NUw2THJyARmJM51M646V2idRWyFFGd8eS",
                "t2FspibzvvMXqyNHmTnstXb4hC2gSiVDuie",
                "t2JgQ1J46knMpy2kRWUYNMtFARhJ6DVfmSr",
                "t2G89D83hUgByjdkm9TjVEdNCaiLkR1tZik",
                "t2JicHfndKzTqHta6Y3F962i6oL6bQf8Jvf",
                "t2U7qyKgyEhiKWidXjtYmo5K5msCEWrgQjc",
                "t2MbDuzDC9mWY7XPdBPAeLdNs3BucWNNF8r",
                "t2BkV39i5s5SVVWf4NEmyKQnGJFNpoZe9Yc",
                "t2Nr6VeZvX81croQ7t6gFbWNwKSHCdp8sf9",
                "t2GLnBThRLKtVmaeXpjQAeHhFND8i2E2MqB",
                "t2TsZC2YUM71DxJaYSzpKFauBe1Vk2mMzNz",
                "t2A4LW1nmTLefFnUqiRvFtMXwEaAfXFyR2j",
                "t2Cko8qCuFLVEumPjiCB4CCrfubnq2XX8So",
                "t2LxqXjysFG1pyQd1hb77yQ7TRrJAZLqxWt",
                "t2UftkxyDZGdXUewqp837Y13aq1mpXM8cCi",
                "t2ExWrmDHfFHe7eoGVNiiWfpyYkydwb35gv",
                "t2PMpwn88gMSrqGnyRrQ3q3FEzQxuffpQ7v",
                "t2T2UtPgp5HsuqgotGZN3KjdeFAtDVjVxeY",
                "t2RY25oaspdJvdqSn9nutCnWwYMLSiDsRpJ",
                "t2C11PWjUg6Yz2uiEoZHZaw8z5DqXaBhJki",
                "t2C7QY9mdgaM2eqSHe1d7w522ZaGsbEgunn",
                "t27PeieZ3S5PSJjVbw6x23drx44zYCeMS4K",
                "t2RUvZ1dJzBHKSW4BmHHJNiRzzNdbs1uMjr",
                "t2LgUP4C6TYCSmATFQpBK8WjHRGZ51w6bgA",
                "t2GgsouwvcGCNNt3BXJZXGW69rhhckTMJTy",
                "t2Kmsts11NdvcUvh71XaDmk3UPhWBAUcnpE",
                "t2NfaAyHYCfuQmD5CWND18BnwEMNwy8QBkh",
                "t29EWyLYaweBazmCSyXNeh4WNP9rzYojtBk",
                "t2J66xPyrC4siK7WtyGjqeXbt7zvyL1h3sb",
                "t2TduW3orSaeAFsEGardWVmFqX6LKVtAXgS",
                "t27sJYiXsehANNzxd1LFCC8CcFtcNEJcMVh",
                "t2GF4sstF42qjCLWRbTgmmiE7XEkXRDrLet",
                "t2Dj2xv7N7uVNv9b952ku1YD4ZFKdBUTBod",
                "t2VbxkxJ6VfPKRKgvbRsUCQoR8GMUk7aGJi",
                "t28KTJXrX9LAp97AZBQnT35APBpLJ6us1XV",
                "t2RhaA1kiAZWcBYEUjHyYhQwvHoFtP1m21V",
                "t2BmWAYcTiteCZY8XvQLnTTXD4GSW7HZooq",
                "t2GEeZauPuCG9jaLNZeFFeqzPsEStzETsf4",
                "t2Etdy8mDsXmvjKkdUwBahMGPh9hHW4wbAj",
                "t28rw9Vwxb4QPTmm9qecikbFE9DFBVcjqTu",
                "t2QHLKFiYQJt6JEPgKfjkvC3yaCjRk5RMz2",
                "t2SWHBQKJZ54sfdPQVkfujcXwzjS1ni4AC8",
                "t27zpAAMu2vEP5njUGdrHJRZWVA4889zyek",
                "t2QCdSBjTzqHKwnefzCD72xTjjfg1cZhze1",
                "t2H3me4Qm9SEWk4QFn8TrZabubuDAg4XYCL",
            };

            // ZF and MG use the same address for each funding period
            std::vector<std::string> zf_addresses(51, "t29yDU4bC9GeSNgJ13gmGsTDzp5Vo8cdKSG");
            std::vector<std::string> mg_addresses(51, "t2AtNsos9WB7Fqx3UjiZEAaa1YgXeCw3ker");

            consensus.AddZIP207FundingStream(
                keyConstants,
                Consensus::FS_ZIP214_BP,
                canopyActivation,
                2796000, // *not* the NU6 activation height
                bp_addresses);
            consensus.AddZIP207FundingStream(
                keyConstants,
                Consensus::FS_ZIP214_ZF,
                canopyActivation,
                2796000, // *not* the NU6 activation height
                zf_addresses);
            consensus.AddZIP207FundingStream(
                keyConstants,
                Consensus::FS_ZIP214_MG,
                canopyActivation,
                2796000, // *not* the NU6 activation height
                mg_addresses);

            // ZIP 214 Revision 1
            // FPF uses a single address repeated 13 times, once for each funding period.
            // There are 13 periods because the start height does not align with a period boundary.
            std::vector<std::string> fpf_addresses(13, "t2LtPHSYNHUKuDauwsjeB8FnBiWMdwLXnAE");
            consensus.AddZIP207FundingStream(
                keyConstants,
                Consensus::FS_FPF_ZCG,
                nu6Activation,
                3396000,
                fpf_addresses);
            consensus.AddZIP207LockboxStream(
                keyConstants,
                Consensus::FS_DEFERRED,
                nu6Activation,
                3396000);

            // ZIP 214 Revision 2
            // FPF uses a single address repeated 27 times, once for each funding period.
            // There are 27 periods because the start height is after the second halving
            // on testnet and does not align with a period boundary.
            std::vector<std::string> fpf_addresses_h3(27, "t2LtPHSYNHUKuDauwsjeB8FnBiWMdwLXnAE");
            consensus.AddZIP207FundingStream(
                keyConstants,
                Consensus::FS_FPF_ZCG_H3,
                nu6_1Activation,
                4476000,
                fpf_addresses_h3);
            consensus.AddZIP207LockboxStream(
                keyConstants,
                Consensus::FS_CCF_H3,
                nu6_1Activation,
                4476000);

            // ZIP 271
            // For testing purposes, we split the lockbox contents into 10 equal chunks.
            std::string nu6_1_kho_address = "t2B2mLHUdBbz7gkdABqdxftoMD5zPymxNjX";
            static const CAmount nu6_1_disbursement_amount = 78750 * COIN;
            static const CAmount nu6_1_chunk_amount = 7875 * COIN;
            static constexpr auto nu6_1_chunks = {
                Consensus::LD_ZIP271_NU6_1_CHUNK_1,
                Consensus::LD_ZIP271_NU6_1_CHUNK_2,
                Consensus::LD_ZIP271_NU6_1_CHUNK_3,
                Consensus::LD_ZIP271_NU6_1_CHUNK_4,
                Consensus::LD_ZIP271_NU6_1_CHUNK_5,
                Consensus::LD_ZIP271_NU6_1_CHUNK_6,
                Consensus::LD_ZIP271_NU6_1_CHUNK_7,
                Consensus::LD_ZIP271_NU6_1_CHUNK_8,
                Consensus::LD_ZIP271_NU6_1_CHUNK_9,
                Consensus::LD_ZIP271_NU6_1_CHUNK_10,
            };
            static_assert(nu6_1_chunk_amount * nu6_1_chunks.size() == nu6_1_disbursement_amount);
            for (auto idx : nu6_1_chunks) {
                consensus.AddZIP271LockboxDisbursement(
                    keyConstants,
                    idx,
                    Consensus::UPGRADE_NU6_1,
                    nu6_1_chunk_amount,
                    nu6_1_kho_address);
            }
        }

        // On testnet we activate this rule 6 blocks after Blossom activation. From block 299188 and
        // prior to Blossom activation, the testnet minimum-difficulty threshold was 15 minutes (i.e.
        // a minimum difficulty block can be mined if no block is mined normally within 15 minutes):
        // <https://zips.z.cash/zip-0205#change-to-difficulty-adjustment-on-testnet>
        // However the median-time-past is 6 blocks behind, and the worst-case time for 7 blocks at a
        // 15-minute spacing is ~105 minutes, which exceeds the limit imposed by the soft fork of
        // 90 minutes.
        //
        // After Blossom, the minimum difficulty threshold time is changed to 6 times the block target
        // spacing, which is 7.5 minutes:
        // <https://zips.z.cash/zip-0208#minimum-difficulty-blocks-on-the-test-network>
        // 7 times that is 52.5 minutes which is well within the limit imposed by the soft fork.

        static_assert(6 * Consensus::POST_BLOSSOM_POW_TARGET_SPACING * 7 < MAX_FUTURE_BLOCK_TIME_MTP - 60,
                      "MAX_FUTURE_BLOCK_TIME_MTP is too low given block target spacing");
        consensus.nFutureTimestampSoftForkHeight = consensus.vUpgrades[Consensus::UPGRADE_BLOSSOM].nActivationHeight + 6;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // Spectre testnet P2P magic ("TSPC")
        pchMessageStart[0] = 0x54;
        pchMessageStart[1] = 0x53;
        pchMessageStart[2] = 0x50;
        pchMessageStart[3] = 0x43;
        nDefaultPort = 18733;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(
            1477648033,
            uint256S("0x0000000000000000000000000000000000000000000000000000000000000006"),
            ParseHex("00a6a51259c3f6732481e2d035197218b7a69504461d04335503cd69759b2d02bd2b53a9653f42cb33c608511c953673fa9da76170958115fe92157ad3bb5720d927f18e09459bf5c6072973e143e20f9bdf0584058c96b7c2234c7565f100d5eea083ba5d3dbaff9f0681799a113e7beff4a611d2b49590563109962baa149b628aae869af791f2f70bb041bd7ebfa658570917f6654a142b05e7ec0289a4f46470be7be5f693b90173eaaa6e84907170f32602204f1f4e1c04b1830116ffd0c54f0b1caa9a5698357bd8aa1f5ac8fc93b405265d824ba0e49f69dab5446653927298e6b7bdc61ee86ff31c07bde86331b4e500d42e4e50417e285502684b7966184505b885b42819a88469d1e9cf55072d7f3510f85580db689302eab377e4e11b14a91fdd0df7627efc048934f0aff8e7eb77eb17b3a95de13678004f2512293891d8baf8dde0ef69be520a58bbd6038ce899c9594cf3e30b8c3d9c7ecc832d4c19a6212747b50724e6f70f6451f78fd27b58ce43ca33b1641304a916186cfbe7dbca224f55d08530ba851e4df22baf7ab7078e9cbea46c0798b35a750f54103b0cdd08c81a6505c4932f6bfbd492a9fced31d54e98b6370d4c96600552fcf5b37780ed18c8787d03200963600db297a8f05dfa551321d17b9917edadcda51e274830749d133ad226f8bb6b94f13b4f77e67b35b71f52112ce9ba5da706ad9573584a2570a4ff25d29ab9761a06bdcf2c33638bf9baf2054825037881c14adf3816ba0cbd0fca689aad3ce16f2fe362c98f48134a9221765d939f0b49677d1c2447e56b46859f1810e2cf23e82a53e0d44f34dae932581b3b7f49eaec59af872cf9de757a964f7b33d143a36c270189508fcafe19398e4d2966948164d40556b05b7ff532f66f5d1edc41334ef742f78221dfe0c7ae2275bb3f24c89ae35f00afeea4e6ed187b866b209dc6e83b660593fce7c40e143beb07ac86c56f39e895385924667efe3a3f031938753c7764a2dbeb0a643fd359c46e614873fd0424e435fa7fac083b9a41a9d6bf7e284eee537ea7c50dd239f359941a43dc982745184bf3ee31a8dc850316aa9c6b66d6985acee814373be3458550659e1a06287c3b3b76a185c5cb93e38c1eebcf34ff072894b6430aed8d34122dafd925c46a515cca79b0269c92b301890ca6b0dc8b679cdac0f23318c105de73d7a46d16d2dad988d49c22e9963c117960bdc70ef0db6b091cf09445a516176b7f6d58ec29539166cc8a38bbff387acefffab2ea5faad0e8bb70625716ef0edf61940733c25993ea3de9f0be23d36e7cb8da10505f9dc426cd0e6e5b173ab4fff8c37e1f1fb56d1ea372013d075e0934c6919393cfc21395eea20718fad03542a4162a9ded66c814ad8320b2d7c2da3ecaf206da34c502db2096d1c46699a91dd1c432f019ad434e2c1ce507f91104f66f491fed37b225b8e0b2888c37276cfa0468fc13b8d593fd9a2675f0f5b20b8a15f8fa7558176a530d6865738ddb25d3426dab905221681cf9da0e0200eea5b2eba3ad3a5237d2a391f9074bf1779a2005cee43eec2b058511532635e0fea61664f531ac2b356f40db5c5d275a4cf5c82d468976455af4e3362cc8f71aa95e71d394aff3ead6f7101279f95bcd8a0fedce1d21cb3c9f6dd3b182fce0db5d6712981b651f29178a24119968b14783cafa713bc5f2a65205a42e4ce9dc7ba462bdb1f3e4553afc15f5f39998fdb53e7e231e3e520a46943734a007c2daa1eda9f495791657eefcac5c32833936e568d06187857ed04d7b97167ae207c5c5ae54e528c36016a984235e9c5b2f0718d7b3aa93c7822ccc772580b6599671b3c02ece8a21399abd33cfd3028790133167d0a97e7de53dc8ff"),
            0x2007ffff, 4, 0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x05a60a92d99d85997cce3b87616c089f6124d7342af37106edc76126334a2c38"));
        assert(genesis.hashMerkleRoot == uint256S("0xc4eaa58879081de3c24a7b117ed2b28300e7ec4c4c1dff1d3f1268b7857a4ddb"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // Spectre testnet seed nodes: add DNS seeds here before testnet launch.

        vFixedSeeds.clear();

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;


        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (0, consensus.hashGenesisBlock),
            0,
            0,
            0
        };

        // Fresh Spectre chain: no legacy value-pool or supply checkpoints.
        nSproutValuePoolCheckpointHeight = 0;
        nSproutValuePoolCheckpointBalance = 0;
        fZIP209Enabled = true;
        hashSproutValuePoolCheckpointBlock.SetNull();
        nChainSupplyCheckpointHeight = 0;
        nChainSupplyCheckpointTotalSupply = 0;
        nChainSupplyCheckpointTransparentValue = 0;
        nChainSupplyCheckpointSproutValue = 0;
        nChainSupplyCheckpointSaplingValue = 0;
        nChainSupplyCheckpointOrchardValue = 0;
        nChainSupplyCheckpointLockboxValue = 0;
        hashChainSupplyCheckpointBlock.SetNull();

        // Founders reward script expects a vector of 2-of-3 multisig addresses
        vFoundersRewardAddress = { "t2BnFkjKYJbW7q4VPTHQaZQXqARtn3RveCQ" };
        assert(vFoundersRewardAddress.size() <= consensus.GetLastFoundersRewardBlockHeight(0));
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        keyConstants.strNetworkID = "regtest";
        strCurrencyUnits = "RSPEC";
        keyConstants.bip44CoinType = 1;
        consensus.fCoinbaseMustBeShielded = false;
        consensus.nSubsidySlowStartInterval = 0;
        consensus.nPreBlossomSubsidyHalvingInterval = Consensus::PRE_BLOSSOM_REGTEST_HALVING_INTERVAL;
        consensus.nPostBlossomSubsidyHalvingInterval = POST_BLOSSOM_HALVING_INTERVAL(Consensus::PRE_BLOSSOM_REGTEST_HALVING_INTERVAL);
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        const size_t N = 48, K = 5;
        static_assert(equihash_parameters_acceptable(N, K));
        consensus.nEquihashN = N;
        consensus.nEquihashK = K;
        consensus.powLimit = uint256S("0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f"); // if this is any larger, the for loop in GetNextWorkRequired can overflow bnTot
        consensus.nPowAveragingWindow = 17;
        assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nPowAveragingWindow);
        consensus.nPowMaxAdjustDown = 0; // Turn off adjustment down
        consensus.nPowMaxAdjustUp = 0; // Turn off adjustment up
        consensus.nPreBlossomPowTargetSpacing = Consensus::PRE_BLOSSOM_POW_TARGET_SPACING;
        consensus.nPostBlossomPowTargetSpacing = Consensus::POST_BLOSSOM_POW_TARGET_SPACING;
        consensus.nPowAllowMinDifficultyBlocksAfterHeight = 0;
        consensus.fPowNoRetargeting = true;
        consensus.vUpgrades[Consensus::BASE_SPROUT].nProtocolVersion = 170002;
        consensus.vUpgrades[Consensus::BASE_SPROUT].nActivationHeight =
            Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nProtocolVersion = 170002;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].nProtocolVersion = 170003;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].nProtocolVersion = 170006;
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_BLOSSOM].nProtocolVersion = 170008;
        consensus.vUpgrades[Consensus::UPGRADE_BLOSSOM].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_HEARTWOOD].nProtocolVersion = 170010;
        consensus.vUpgrades[Consensus::UPGRADE_HEARTWOOD].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nProtocolVersion = 170012;
        consensus.vUpgrades[Consensus::UPGRADE_CANOPY].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_NU5].nProtocolVersion = 170050;
        consensus.vUpgrades[Consensus::UPGRADE_NU5].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_NU6].nProtocolVersion = 170110;
        consensus.vUpgrades[Consensus::UPGRADE_NU6].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_NU6_1].nProtocolVersion = 170130;
        consensus.vUpgrades[Consensus::UPGRADE_NU6_1].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.nTemporaryOrchardDisablingSoftForkHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_NU6_2].nProtocolVersion = 170150;
        consensus.vUpgrades[Consensus::UPGRADE_NU6_2].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_ZFUTURE].nProtocolVersion = 0x7FFFFFFF;
        consensus.vUpgrades[Consensus::UPGRADE_ZFUTURE].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;

        consensus.nFundingPeriodLength = consensus.nPostBlossomSubsidyHalvingInterval / 48;
        // Defined funding streams can be enabled with node config flags.

        // These prefixes are the same as the testnet prefixes
        keyConstants.base58Prefixes[PUBKEY_ADDRESS]     = {0x1D,0x25};
        keyConstants.base58Prefixes[SCRIPT_ADDRESS]     = {0x1C,0xBA};
        keyConstants.base58Prefixes[SECRET_KEY]         = {0xEF};
        // do not rely on these BIP32 prefixes; they are not specified and may change
        keyConstants.base58Prefixes[EXT_PUBLIC_KEY]     = {0x04,0x35,0x87,0xCF};
        keyConstants.base58Prefixes[EXT_SECRET_KEY]     = {0x04,0x35,0x83,0x94};
        keyConstants.base58Prefixes[ZCPAYMENT_ADDRESS]  = {0x16,0xB6};
        keyConstants.base58Prefixes[ZCVIEWING_KEY]      = {0xA8,0xAC,0x0C};
        keyConstants.base58Prefixes[ZCSPENDING_KEY]     = {0xAC,0x08};

        keyConstants.bech32HRPs[SAPLING_PAYMENT_ADDRESS]      = "rspec";
        keyConstants.bech32HRPs[SAPLING_FULL_VIEWING_KEY]     = "rspecviews";
        keyConstants.bech32HRPs[SAPLING_INCOMING_VIEWING_KEY] = "rspecivks";
        keyConstants.bech32HRPs[SAPLING_EXTENDED_SPEND_KEY]   = "secret-extended-key-rspec";
        keyConstants.bech32HRPs[SAPLING_EXTENDED_FVK]         = "rxspecviews";

        keyConstants.bech32mHRPs[TEX_ADDRESS]                 = "rstex";

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        pchMessageStart[0] = 0xaa;
        pchMessageStart[1] = 0xe8;
        pchMessageStart[2] = 0x3f;
        pchMessageStart[3] = 0x5f;
        nDefaultPort = 18344;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(
            1296688602,
            uint256S("0x0000000000000000000000000000000000000000000000000000000000000009"),
            ParseHex("01936b7db1eb4ac39f151b8704642d0a8bda13ec547d54cd5e43ba142fc6d8877cab07b3"),
            0x200f0f0f, 4, 0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x029f11d80ef9765602235e1bc9727e3eb6ba20839319f761fee920d63401e327"));
        assert(genesis.hashMerkleRoot == uint256S("0xc4eaa58879081de3c24a7b117ed2b28300e7ec4c4c1dff1d3f1268b7857a4ddb"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData){
            boost::assign::map_list_of
            ( 0, uint256S("0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206")),
            0,
            0,
            0
        };

        // Founders reward script expects a vector of 2-of-3 multisig addresses
        vFoundersRewardAddress = { "t2FwcEhFdNXuFMv1tcYwaBJtYVtMj8b1uTg" };
        assert(vFoundersRewardAddress.size() <= consensus.GetLastFoundersRewardBlockHeight(0));

        // do not require the wallet backup to be confirmed in regtest mode
        fRequireWalletBackup = false;
    }

    void UpdateNetworkUpgradeParameters(Consensus::UpgradeIndex idx, int nActivationHeight)
    {
        assert(idx > Consensus::BASE_SPROUT && idx < Consensus::MAX_NETWORK_UPGRADES);
        consensus.vUpgrades[idx].nActivationHeight = nActivationHeight;
    }

    void UpdateFundingStreamParameters(Consensus::FundingStreamIndex idx, Consensus::FundingStream fs)
    {
        assert(idx >= Consensus::FIRST_FUNDING_STREAM && idx < Consensus::MAX_FUNDING_STREAMS);
        consensus.vFundingStreams[idx] = fs;
    }

    void UpdateOnetimeLockboxDisbursementParameters(
        Consensus::OnetimeLockboxDisbursementIndex idx,
        Consensus::OnetimeLockboxDisbursement ld)
    {
        assert(idx >= Consensus::FIRST_ONETIME_LOCKBOX_DISBURSEMENT && idx < Consensus::MAX_ONETIME_LOCKBOX_DISBURSEMENTS);
        consensus.vOnetimeLockboxDisbursements[idx] = ld;
    }

    void UpdateRegtestPow(
        int64_t nPowMaxAdjustDown,
        int64_t nPowMaxAdjustUp,
        uint256 powLimit,
        bool noRetargeting)
    {
        consensus.nPowMaxAdjustDown = nPowMaxAdjustDown;
        consensus.nPowMaxAdjustUp = nPowMaxAdjustUp;
        consensus.powLimit = powLimit;
        consensus.fPowNoRetargeting = noRetargeting;
    }

    void UpdateTemporaryOrchardDisablingSoftForkHeight(int nHeight)
    {
        consensus.nTemporaryOrchardDisablingSoftForkHeight = nHeight;
    }

    void SetRegTestZIP209Enabled() {
        fZIP209Enabled = true;
    }

    void SetRegTestAllowLegacyChainSupplyData() {
        fRegTestAllowLegacyChainSupplyData = true;
    }

    void SetRegTestChainSupplyCheckpoint() {
        // Hardcoded checkpoint at height 200 (the tip of the regtest caches).
        // The block hash is left null; FallbackChainSupplyCheckpoint
        // skips the hash check when it is null.
        // Values from the sprout regtest cache at height 200.
        // The sprout cache has 4 × 50 ZEC = 200 ZEC shielded into Sprout.
        nChainSupplyCheckpointHeight = 200;
        nChainSupplyCheckpointTotalSupply = 214375000000;
        nChainSupplyCheckpointTransparentValue = 194375000000;
        nChainSupplyCheckpointSproutValue = 20000000000;
        nChainSupplyCheckpointSaplingValue = 0;
        nChainSupplyCheckpointOrchardValue = 0;
        nChainSupplyCheckpointLockboxValue = 0;
        hashChainSupplyCheckpointBlock.SetNull();
    }
};
static CRegTestParams regTestParams;

static const CChainParams* pCurrentParams = nullptr;

const CChainParams& Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

const CChainParams& Params(const std::string& chain)
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

    // Some python qa rpc tests need to enforce the coinbase consensus rule
    if (network == CBaseChainParams::REGTEST && mapArgs.count("-regtestshieldcoinbase")) {
        regTestParams.SetRegTestCoinbaseMustBeShielded();
    }

    // When a developer is debugging turnstile violations in regtest mode, enable ZIP209
    if (network == CBaseChainParams::REGTEST && mapArgs.count("-developersetpoolsizezero")) {
        regTestParams.SetRegTestZIP209Enabled();
    }

    // Allow regtest tests that use legacy block index data (lacking nChainSupplyDelta)
    // to skip the supply consistency check rather than aborting the node.
    if (network == CBaseChainParams::REGTEST && mapArgs.count("-regtestallowlegacychainsupplydata")) {
        regTestParams.SetRegTestAllowLegacyChainSupplyData();
    }

    if (network == CBaseChainParams::REGTEST && mapArgs.count("-regtestchainsupplycheckpoint")) {
        regTestParams.SetRegTestChainSupplyCheckpoint();
    }

    // Enable ZIP 209 enforcement without zeroing shielded pool balances.
    if (network == CBaseChainParams::REGTEST && mapArgs.count("-regtestenablezip209")) {
        regTestParams.SetRegTestZIP209Enabled();
    }
}


// Block height must be >0 and <=last founders reward block height
// Index variable i ranges from 0 - (vFoundersRewardAddress.size()-1)
std::string CChainParams::GetFoundersRewardAddressAtHeight(int nHeight) const {
    int preBlossomMaxHeight = consensus.GetLastFoundersRewardBlockHeight(0);
    // zip208
    // FounderAddressAdjustedHeight(height) :=
    // height, if not IsBlossomActivated(height)
    // BlossomActivationHeight + floor((height - BlossomActivationHeight) / BlossomPoWTargetSpacingRatio), otherwise
    bool blossomActive = consensus.NetworkUpgradeActive(nHeight, Consensus::UPGRADE_BLOSSOM);
    if (blossomActive) {
        int blossomActivationHeight = consensus.vUpgrades[Consensus::UPGRADE_BLOSSOM].nActivationHeight;
        nHeight = blossomActivationHeight + ((nHeight - blossomActivationHeight) / Consensus::BLOSSOM_POW_TARGET_SPACING_RATIO);
    }
    assert(nHeight > 0 && nHeight <= preBlossomMaxHeight);
    size_t addressChangeInterval = (preBlossomMaxHeight + vFoundersRewardAddress.size()) / vFoundersRewardAddress.size();
    size_t i = nHeight / addressChangeInterval;
    return vFoundersRewardAddress[i];
}

// Block height must be >0 and <=last founders reward block height
// The founders reward address is expected to be a multisig (P2SH) address
CScript CChainParams::GetFoundersRewardScriptAtHeight(int nHeight) const {
    assert(nHeight > 0 && nHeight <= consensus.GetLastFoundersRewardBlockHeight(nHeight));

    KeyIO keyIO(*this);
    auto address = keyIO.DecodePaymentAddress(GetFoundersRewardAddressAtHeight(nHeight).c_str());
    assert(address.has_value());
    assert(std::holds_alternative<CScriptID>(address.value()));
    CScriptID scriptID = std::get<CScriptID>(address.value());
    CScript script = CScript() << OP_HASH160 << ToByteVector(scriptID) << OP_EQUAL;
    return script;
}

std::string CChainParams::GetFoundersRewardAddressAtIndex(int i) const {
    assert(i >= 0 && i < vFoundersRewardAddress.size());
    return vFoundersRewardAddress[i];
}

void UpdateNetworkUpgradeParameters(Consensus::UpgradeIndex idx, int nActivationHeight)
{
    regTestParams.UpdateNetworkUpgradeParameters(idx, nActivationHeight);
}

void UpdateFundingStreamParameters(Consensus::FundingStreamIndex idx, Consensus::FundingStream fs)
{
    regTestParams.UpdateFundingStreamParameters(idx, fs);
}

void UpdateOnetimeLockboxDisbursementParameters(
    Consensus::OnetimeLockboxDisbursementIndex idx,
    Consensus::OnetimeLockboxDisbursement ld)
{
    regTestParams.UpdateOnetimeLockboxDisbursementParameters(idx, ld);
}

void UpdateRegtestPow(
    int64_t nPowMaxAdjustDown,
    int64_t nPowMaxAdjustUp,
    uint256 powLimit,
    bool noRetargeting)
{
    regTestParams.UpdateRegtestPow(nPowMaxAdjustDown, nPowMaxAdjustUp, powLimit, noRetargeting);
}

void UpdateRegtestTemporaryOrchardDisablingSoftForkHeight(int nHeight)
{
    regTestParams.UpdateTemporaryOrchardDisablingSoftForkHeight(nHeight);
}
