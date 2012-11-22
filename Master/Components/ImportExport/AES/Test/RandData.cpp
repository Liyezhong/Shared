/****************************************************************************/
/** @file RandData.cpp
 *
 *  @brief Test data for AES random data tests.
 *
 *  The data are taken from the original NIST submission of AES, file tbl.txt.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-06-17
 *  $Author:    $ R.Wobst
 *
 *  @b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 *  last modified by owner: @(#) Aug 24 2011, 12:48:16
 *
 */
/****************************************************************************/

#include "ImportExport/AES/Test/TestAES.h"

namespace ImportExport {

RANDTEST RandData[] =
{
    {"00010203050607080a0b0c0d0f101112",
        "506812a45f08c889b97f5980038b8359",
        "d8f532538289ef7d06b506a4fd5be9c9"},

    {"14151617191a1b1c1e1f202123242526",
        "5c6d71ca30de8b8b00549984d2ec7d4b",
        "59ab30f4d4ee6e4ff9907ef65b1fb68c"},

    {"28292a2b2d2e2f30323334353738393a",
        "53f3f4c64f8616e4e7c56199f48f21f6",
        "bf1ed2fcb2af3fd41443b56d85025cb1"},

    {"3c3d3e3f41424344464748494b4c4d4e",
        "a1eb65a3487165fb0f1c27ff9959f703",
        "7316632d5c32233edcb0780560eae8b2"},

    {"50515253555657585a5b5c5d5f606162",
        "3553ecf0b1739558b08e350a98a39bfa",
        "408c073e3e2538072b72625e68b8364b"},

    {"64656667696a6b6c6e6f707173747576",
        "67429969490b9711ae2b01dc497afde8",
        "e1f94dfa776597beaca262f2f6366fea"},

    {"78797a7b7d7e7f80828384858788898a",
        "93385c1f2aec8bed192f5a8e161dd508",
        "f29e986c6a1c27d7b29ffd7ee92b75f1"},

    {"8c8d8e8f91929394969798999b9c9d9e",
        "b5bf946be19beb8db3983b5f4c6e8ddb",
        "131c886a57f8c2e713aba6955e2b55b5"},

    {"a0a1a2a3a5a6a7a8aaabacadafb0b1b2",
        "41321ee10e21bd907227c4450ff42324",
        "d2ab7662df9b8c740210e5eeb61c199d"},

    {"b4b5b6b7b9babbbcbebfc0c1c3c4c5c6",
        "00a82f59c91c8486d12c0a80124f6089",
        "14c10554b2859c484cab5869bbe7c470"},

    {"c8c9cacbcdcecfd0d2d3d4d5d7d8d9da",
        "7ce0fd076754691b4bbd9faf8a1372fe",
        "db4d498f0a49cf55445d502c1f9ab3b5"},

    {"dcdddedfe1e2e3e4e6e7e8e9ebecedee",
        "23605a8243d07764541bc5ad355b3129",
        "6d96fef7d66590a77a77bb2056667f7f"},

    {"f0f1f2f3f5f6f7f8fafbfcfdfe010002",
        "12a8cfa23ea764fd876232b4e842bc44",
        "316fb68edba736c53e78477bf913725c"},

    {"04050607090a0b0c0e0f101113141516",
        "bcaf32415e8308b3723e5fdd853ccc80",
        "6936f2b93af8397fd3a771fc011c8c37"},

    {"2c2d2e2f31323334363738393b3c3d3e",
        "89afae685d801ad747ace91fc49adde0",
        "f3f92f7a9c59179c1fcc2c2ba0b082cd"},

    {"40414243454647484a4b4c4d4f505152",
        "f521d07b484357c4a69e76124a634216",
        "6a95ea659ee3889158e7a9152ff04ebc"},

    {"54555657595a5b5c5e5f606163646566",
        "3e23b3bc065bcc152407e23896d77783",
        "1959338344e945670678a5d432c90b93"},

    {"68696a6b6d6e6f70727374757778797a",
        "79f0fba002be1744670e7e99290d8f52",
        "e49bddd2369b83ee66e6c75a1161b394"},

    {"7c7d7e7f81828384868788898b8c8d8e",
        "da23fe9d5bd63e1d72e3dafbe21a6c2a",
        "d3388f19057ff704b70784164a74867d"},

    {"a4a5a6a7a9aaabacaeafb0b1b3b4b5b6",
        "e3f5698ba90b6a022efd7db2c7e6c823",
        "23aa03e2d5e4cd24f3217e596480d1e1"},

    {"e0e1e2e3e5e6e7e8eaebecedeff0f1f2",
        "bdc2691d4f1b73d2700679c3bcbf9c6e",
        "c84113d68b666ab2a50a8bdb222e91b9"},

    {"08090a0b0d0e0f10121314151718191a",
        "ba74e02093217ee1ba1b42bd5624349a",
        "ac02403981cd4340b507963db65cb7b6"},

    {"6c6d6e6f71727374767778797b7c7d7e",
        "b5c593b5851c57fbf8b3f57715e8f680",
        "8d1299236223359474011f6bf5088414"},

    {"80818283858687888a8b8c8d8f909192",
        "3da9bd9cec072381788f9387c3bbf4ee",
        "5a1d6ab8605505f7977e55b9a54d9b90"},

    {"94959697999a9b9c9e9fa0a1a3a4a5a6",
        "4197f3051121702ab65d316b3c637374",
        "72e9c2d519cf555e4208805aabe3b258"},

    {"a8a9aaabadaeafb0b2b3b4b5b7b8b9ba",
        "9f46c62ec4f6ee3f6e8c62554bc48ab7",
        "a8f3e81c4a23a39ef4d745dffe026e80"},

    {"bcbdbebfc1c2c3c4c6c7c8c9cbcccdce",
        "0220673fe9e699a4ebc8e0dbeb6979c8",
        "546f646449d31458f9eb4ef5483aee6c"},

    {"d0d1d2d3d5d6d7d8dadbdcdddfe0e1e2",
        "b2b99171337ded9bc8c2c23ff6f18867",
        "4dbe4bc84ac797c0ee4efb7f1a07401c"},

    {"e4e5e6e7e9eaebeceeeff0f1f3f4f5f6",
        "a7facf4e301e984e5efeefd645b23505",
        "25e10bfb411bbd4d625ac8795c8ca3b3"},

    {"f8f9fafbfdfefe00020304050708090a",
        "f7c762e4a9819160fd7acfb6c4eedcdd",
        "315637405054ec803614e43def177579"},

    {"0c0d0e0f11121314161718191b1c1d1e",
        "9b64fc21ea08709f4915436faa70f1be",
        "60c5bc8a1410247295c6386c59e572a8"},

    {"20212223252627282a2b2c2d2f303132",
        "52af2c3de07ee6777f55a4abfc100b3f",
        "01366fc8ca52dfe055d6a00a76471ba6"},

    {"34353637393a3b3c3e3f404143444546",
        "2fca001224386c57aa3f968cbe2c816f",
        "ecc46595516ec612449c3f581e7d42ff"},

    {"48494a4b4d4e4f50525354555758595a",
        "4149c73658a4a9c564342755ee2c132f",
        "6b7ffe4c602a154b06ee9c7dab5331c9"},

    {"5c5d5e5f61626364666768696b6c6d6e",
        "af60005a00a1772f7c07a48a923c23d2",
        "7da234c14039a240dd02dd0fbf84eb67"},

    {"70717273757677787a7b7c7d7f808182",
        "6fccbc28363759914b6f0280afaf20c6",
        "c7dc217d9e3604ffe7e91f080ecd5a3a"},

    {"84858687898a8b8c8e8f909193949596",
        "7d82a43ddf4fefa2fc5947499884d386",
        "37785901863f5c81260ea41e7580cda5"},

    {"98999a9b9d9e9fa0a2a3a4a5a7a8a9aa",
        "5d5a990eaab9093afe4ce254dfa49ef9",
        "a07b9338e92ed105e6ad720fccce9fe4"},

    {"acadaeafb1b2b3b4b6b7b8b9bbbcbdbe",
        "4cd1e2fd3f4434b553aae453f0ed1a02",
        "ae0fb9722418cc21a7da816bbc61322c"},

    {"c0c1c2c3c5c6c7c8cacbcccdcfd0d1d2",
        "5a2c9a9641d4299125fa1b9363104b5e",
        "c826a193080ff91ffb21f71d3373c877"},

    {"d4d5d6d7d9dadbdcdedfe0e1e3e4e5e6",
        "b517fe34c0fa217d341740bfd4fe8dd4",
        "1181b11b0e494e8d8b0aa6b1d5ac2c48"},

    {"e8e9eaebedeeeff0f2f3f4f5f7f8f9fa",
        "014baf2278a69d331d5180103643e99a",
        "6743c3d1519ab4f2cd9a78ab09a511bd"},

    {"fcfdfeff01020304060708090b0c0d0e",
        "b529bd8164f20d0aa443d4932116841c",
        "dc55c076d52bacdf2eefd952946a439d"},

    {"10111213151617181a1b1c1d1f202122",
        "2e596dcbb2f33d4216a1176d5bd1e456",
        "711b17b590ffc72b5c8e342b601e8003"},

    {"24252627292a2b2c2e2f303133343536",
        "7274a1ea2b7ee2424e9a0e4673689143",
        "19983bb0950783a537e1339f4aa21c75"},

    {"38393a3b3d3e3f40424344454748494a",
        "ae20020bd4f13e9d90140bee3b5d26af",
        "3ba7762e15554169c0f4fa39164c410c"},

    {"4c4d4e4f51525354565758595b5c5d5e",
        "baac065da7ac26e855e79c8849d75a02",
        "a0564c41245afca7af8aa2e0e588ea89"},

    {"60616263656667686a6b6c6d6f707172",
        "7c917d8d1d45fab9e2540e28832540cc",
        "5e36a42a2e099f54ae85ecd92e2381ed"},

    {"74757677797a7b7c7e7f808183848586",
        "bde6f89e16daadb0e847a2a614566a91",
        "770036f878cd0f6ca2268172f106f2fe"},

    {"88898a8b8d8e8f90929394959798999a",
        "c9de163725f1f5be44ebb1db51d07fbc",
        "7e4e03908b716116443ccf7c94e7c259"},

    {"9c9d9e9fa1a2a3a4a6a7a8a9abacadae",
        "3af57a58f0c07dffa669572b521e2b92",
        "482735a48c30613a242dd494c7f9185d"},

    {"b0b1b2b3b5b6b7b8babbbcbdbfc0c1c2",
        "3d5ebac306dde4604f1b4fbbbfcdae55",
        "b4c0f6c9d4d7079addf9369fc081061d"},

    {"c4c5c6c7c9cacbcccecfd0d1d3d4d5d6",
        "c2dfa91bceb76a1183c995020ac0b556",
        "d5810fe0509ac53edcd74f89962e6270"},

    {"d8d9dadbdddedfe0e2e3e4e5e7e8e9ea",
        "c70f54305885e9a0746d01ec56c8596b",
        "03f17a16b3f91848269ecdd38ebb2165"},

    {"ecedeeeff1f2f3f4f6f7f8f9fbfcfdfe",
        "c4f81b610e98012ce000182050c0c2b2",
        "da1248c3180348bad4a93b4d9856c9df"},

    {"00010203050607080a0b0c0d0f101112",
        "eaab86b1d02a95d7404eff67489f97d4",
        "3d10d7b63f3452c06cdf6cce18be0c2c"},

    {"14151617191a1b1c1e1f202123242526",
        "7c55bdb40b88870b52bec3738de82886",
        "4ab823e7477dfddc0e6789018fcb6258"},

    {"28292a2b2d2e2f30323334353738393a",
        "ba6eaa88371ff0a3bd875e3f2a975ce0",
        "e6478ba56a77e70cfdaa5c843abde30e"},

    {"3c3d3e3f41424344464748494b4c4d4e",
        "08059130c4c24bd30cf0575e4e0373dc",
        "1673064895fbeaf7f09c5429ff75772d"},

    {"50515253555657585a5b5c5d5f606162",
        "9a8eab004ef53093dfcf96f57e7eda82",
        "4488033ae9f2efd0ca9383bfca1a94e9"},

    {"64656667696a6b6c6e6f707173747576",
        "0745b589e2400c25f117b1d796c28129",
        "978f3b8c8f9d6f46626cac3c0bcb9217"},

    {"78797a7b7d7e7f80828384858788898a",
        "2f1777781216cec3f044f134b1b92bbe",
        "e08c8a7e582e15e5527f1d9e2eecb236"},

    {"8c8d8e8f91929394969798999b9c9d9e",
        "353a779ffc541b3a3805d90ce17580fc",
        "cec155b76ac5ffda4cf4f9ca91e49a7a"},

    {"a0a1a2a3a5a6a7a8aaabacadafb0b1b2",
        "1a1eae4415cefcf08c4ac1c8f68bea8f",
        "d5ac7165763225dd2a38cdc6862c29ad"},

    {"b4b5b6b7b9babbbcbebfc0c1c3c4c5c6",
        "e6e7e4e5b0b3b2b5d4d5aaab16111013",
        "03680fe19f7ce7275452020be70e8204"},

    {"c8c9cacbcdcecfd0d2d3d4d5d7d8d9da",
        "f8f9fafbfbf8f9e677767170efe0e1e2",
        "461df740c9781c388e94bb861ceb54f6"},

    {"dcdddedfe1e2e3e4e6e7e8e9ebecedee",
        "63626160a1a2a3a445444b4a75727370",
        "451bd60367f96483042742219786a074"},

    {"f0f1f2f3f5f6f7f8fafbfcfdfe010002",
        "717073720605040b2d2c2b2a05fafbf9",
        "e4dfa42671a02e57ef173b85c0ea9f2b"},

    {"04050607090a0b0c0e0f101113141516",
        "78797a7beae9e8ef3736292891969794",
        "ed11b89e76274282227d854700a78b9e"},

    {"18191a1b1d1e1f20222324252728292a",
        "838281803231300fdddcdbdaa0afaead",
        "433946eaa51ea47af33895f2b90b3b75"},

    {"2c2d2e2f31323334363738393b3c3d3e",
        "18191a1bbfbcbdba75747b7a7f78797a",
        "6bc6d616a5d7d0284a5910ab35022528"},

    {"40414243454647484a4b4c4d4f505152",
        "848586879b989996a3a2a5a4849b9a99",
        "d2a920ecfe919d354b5f49eae9719c98"},

    {"54555657595a5b5c5e5f606163646566",
        "0001020322212027cacbf4f551565754",
        "3a061b17f6a92885efbd0676985b373d"},

    {"68696a6b6d6e6f70727374757778797a",
        "cecfcccdafacadb2515057564a454447",
        "fadeec16e33ea2f4688499d157e20d8f"},

    {"7c7d7e7f81828384868788898b8c8d8e",
        "92939091cdcecfc813121d1c80878685",
        "5cdefede59601aa3c3cda36fa6b1fa13"},

    {"90919293959697989a9b9c9d9fa0a1a2",
        "d2d3d0d16f6c6d6259585f5ed1eeefec",
        "9574b00039844d92ebba7ee8719265f8"},

    {"a4a5a6a7a9aaabacaeafb0b1b3b4b5b6",
        "acadaeaf878485820f0e1110d5d2d3d0",
        "9a9cf33758671787e5006928188643fa"},

    {"b8b9babbbdbebfc0c2c3c4c5c7c8c9ca",
        "9091929364676619e6e7e0e1757a7b78",
        "2cddd634c846ba66bb46cbfea4a674f9"},

    {"cccdcecfd1d2d3d4d6d7d8d9dbdcddde",
        "babbb8b98a89888f74757a7b92959497",
        "d28bae029393c3e7e26e9fafbbb4b98f"},

    {"e0e1e2e3e5e6e7e8eaebecedeff0f1f2",
        "8d8c8f8e6e6d6c633b3a3d3ccad5d4d7",
        "ec27529b1bee0a9ab6a0d73ebc82e9b7"},

    {"f4f5f6f7f9fafbfcfefe010103040506",
        "86878485010203040808f7f767606162",
        "3cb25c09472aff6ee7e2b47ccd7ccb17"},

    {"08090a0b0d0e0f10121314151718191a",
        "8e8f8c8d656667788a8b8c8d010e0f0c",
        "dee33103a7283370d725e44ca38f8fe5"},

    {"1c1d1e1f21222324262728292b2c2d2e",
        "c8c9cacb858687807a7b7475e7e0e1e2",
        "27f9bcd1aac64bffc11e7815702c1a69"},

    {"30313233353637383a3b3c3d3f404142",
        "6d6c6f6e5053525d8c8d8a8badd2d3d0",
        "5df534ffad4ed0749a9988e9849d0021"},

    {"44454647494a4b4c4e4f505153545556",
        "28292a2b393a3b3c0607181903040506",
        "a48bee75db04fb60ca2b80f752a8421b"},

    {"58595a5b5d5e5f60626364656768696a",
        "a5a4a7a6b0b3b28ddbdadddcbdb2b3b0",
        "024c8cf70bc86ee5ce03678cb7af45f9"},

    {"6c6d6e6f71727374767778797b7c7d7e",
        "323330316467666130313e3f2c2b2a29",
        "3c19ac0f8a3a3862ce577831301e166b"},

    {"80818283858687888a8b8c8d8f909192",
        "27262524080b0a05171611100b141516",
        "c5e355b796a57421d59ca6be82e73bca"},

    {"94959697999a9b9c9e9fa0a1a3a4a5a6",
        "040506074142434435340b0aa3a4a5a6",
        "d94033276417abfb05a69d15b6e386e2"},

    {"a8a9aaabadaeafb0b2b3b4b5b7b8b9ba",
        "242526271112130c61606766bdb2b3b0",
        "24b36559ea3a9b9b958fe6da3e5b8d85"},

    {"bcbdbebfc1c2c3c4c6c7c8c9cbcccdce",
        "4b4a4948252627209e9f9091cec9c8cb",
        "20fd4feaa0e8bf0cce7861d74ef4cb72"},

    {"d0d1d2d3d5d6d7d8dadbdcdddfe0e1e2",
        "68696a6b6665646b9f9e9998d9e6e7e4",
        "350e20d5174277b9ec314c501570a11d"},

    {"e4e5e6e7e9eaebeceeeff0f1f3f4f5f6",
        "34353637c5c6c7c0f0f1eeef7c7b7a79",
        "87a29d61b7c604d238fe73045a7efd57"},

    {"f8f9fafbfdfefe00020304050708090a",
        "32333031c2c1c13f0d0c0b0a050a0b08",
        "2c3164c1cc7d0064816bdc0faa362c52"},

    {"0c0d0e0f11121314161718191b1c1d1e",
        "cdcccfcebebdbcbbabaaa5a4181f1e1d",
        "195fe5e8a05a2ed594f6e4400eee10b3"},

    {"20212223252627282a2b2c2d2f303132",
        "212023223635343ba0a1a6a7445b5a59",
        "e4663df19b9a21a5a284c2bd7f905025"},

    {"34353637393a3b3c3e3f404143444546",
        "0e0f0c0da8abaaad2f2e515002050407",
        "21b88714cfb4e2a933bd281a2c4743fd"},

    {"48494a4b4d4e4f50525354555758595a",
        "070605042a2928378e8f8889bdb2b3b0",
        "cbfc3980d704fd0fc54378ab84e17870"},

    {"5c5d5e5f61626364666768696b6c6d6e",
        "cbcac9c893909196a9a8a7a6a5a2a3a0",
        "bc5144baa48bdeb8b63e22e03da418ef"},

    {"70717273757677787a7b7c7d7f808182",
        "80818283c1c2c3cc9c9d9a9b0cf3f2f1",
        "5a1dbaef1ee2984b8395da3bdffa3ccc"},

    {"84858687898a8b8c8e8f909193949596",
        "1213101125262720fafbe4e5b1b6b7b4",
        "f0b11cd0729dfcc80cec903d97159574"},

    {"98999a9b9d9e9fa0a2a3a4a5a7a8a9aa",
        "7f7e7d7c3033320d97969190222d2c2f",
        "9f95314acfddc6d1914b7f19a9cc8209"},

    {"acadaeafb1b2b3b4b6b7b8b9bbbcbdbe",
        "4e4f4c4d484b4a4d81808f8e53545556",
        "595736f6f0f70914a94e9e007f022519"},

    {"c0c1c2c3c5c6c7c8cacbcccdcfd0d1d2",
        "dcdddedfb0b3b2bd15141312a1bebfbc",
        "1f19f57892cae586fcdfb4c694deb183"},

    {"d4d5d6d7d9dadbdcdedfe0e1e3e4e5e6",
        "93929190282b2a2dc4c5fafb92959497",
        "540700ee1f6f3dab0b3eddf6caee1ef5"},

    {"e8e9eaebedeeeff0f2f3f4f5f7f8f9fa",
        "f5f4f7f6c4c7c6d9373631307e717073",
        "14a342a91019a331687a2254e6626ca2"},

    {"fcfdfeff01020304060708090b0c0d0e",
        "93929190b6b5b4b364656a6b05020300",
        "7b25f3c3b2eea18d743ef283140f29ff"},

    {"10111213151617181a1b1c1d1f202122",
        "babbb8b90d0e0f00a4a5a2a3043b3a39",
        "46c2587d66e5e6fa7f7ca6411ad28047"},

    {"24252627292a2b2c2e2f303133343536",
        "d8d9dadb7f7c7d7a10110e0f787f7e7d",
        "09470e72229d954ed5ee73886dfeeba9"},

    {"38393a3b3d3e3f40424344454748494a",
        "fefffcfdefeced923b3a3d3c6768696a",
        "d77c03de92d4d0d79ef8d4824ef365eb"},

    {"4c4d4e4f51525354565758595b5c5d5e",
        "d6d7d4d58a89888f96979899a5a2a3a0",
        "1d190219f290e0f1715d152d41a23593"},

    {"60616263656667686a6b6c6d6f707172",
        "18191a1ba8abaaa5303136379b848586",
        "a2cd332ce3a0818769616292e87f757b"},

    {"74757677797a7b7c7e7f808183848586",
        "6b6a6968a4a7a6a1d6d72829b0b7b6b5",
        "d54afa6ce60fbf9341a3690e21385102"},

    {"88898a8b8d8e8f90929394959798999a",
        "000102038a89889755545352a6a9a8ab",
        "06e5c364ded628a3f5e05e613e356f46"},

    {"9c9d9e9fa1a2a3a4a6a7a8a9abacadae",
        "2d2c2f2eb3b0b1b6b6b7b8b9f2f5f4f7",
        "eae63c0e62556dac85d221099896355a"},

    {"b0b1b2b3b5b6b7b8babbbcbdbfc0c1c2",
        "979695943536373856575051e09f9e9d",
        "1fed060e2c6fc93ee764403a889985a2"},

    {"c4c5c6c7c9cacbcccecfd0d1d3d4d5d6",
        "a4a5a6a7989b9a9db1b0afae7a7d7c7f",
        "c25235c1a30fdec1c7cb5c5737b2a588"},

    {"d8d9dadbdddedfe0e2e3e4e5e7e8e9ea",
        "c1c0c3c2686b6a55a8a9aeafeae5e4e7",
        "796dbef95147d4d30873ad8b7b92efc0"},

    {"ecedeeeff1f2f3f4f6f7f8f9fbfcfdfe",
        "c1c0c3c2141716118c8d828364636261",
        "cbcf0fb34d98d0bd5c22ce37211a46bf"},

    {"00010203050607080a0b0c0d0f101112",
        "93929190cccfcec196979091e0fffefd",
        "94b44da6466126cafa7c7fd09063fc24"},

    {"14151617191a1b1c1e1f202123242526",
        "b4b5b6b7f9fafbfc25241b1a6e69686b",
        "d78c5b5ebf9b4dbda6ae506c5074c8fe"},

    {"28292a2b2d2e2f30323334353738393a",
        "868784850704051ac7c6c1c08788898a",
        "6c27444c27204b043812cf8cf95f9769"},

    {"3c3d3e3f41424344464748494b4c4d4e",
        "f4f5f6f7aaa9a8affdfcf3f277707172",
        "be94524ee5a2aa50bba8b75f4c0aebcf"},

    {"50515253555657585a5b5c5d5f606162",
        "d3d2d1d00605040bc3c2c5c43e010003",
        "a0aeaae91ba9f31f51aeb3588cf3a39e"},

    {"64656667696a6b6c6e6f707173747576",
        "73727170424140476a6b74750d0a0b08",
        "275297779c28266ef9fe4c6a13c08488"},

    {"78797a7b7d7e7f80828384858788898a",
        "c2c3c0c10a0908f754555253a1aeafac",
        "86523d92bb8672cb01cf4a77fd725882"},

    {"8c8d8e8f91929394969798999b9c9d9e",
        "6d6c6f6ef8fbfafd82838c8df8fffefd",
        "4b8327640e9f33322a04dd96fcbf9a36"},

    {"a0a1a2a3a5a6a7a8aaabacadafb0b1b2",
        "f5f4f7f684878689a6a7a0a1d2cdcccf",
        "ce52af650d088ca559425223f4d32694"}
};

int RandSize = sizeof(RandData)/sizeof(RANDTEST);

}                       // end namespace ImportExport
