#Update as of 07/05/2016

Today is a bitter sweet day as on one hand it marks the first alpha release of RingToken client and on the other hand today  is the day RingToken is officially abandoned. See https://www.reddit.com/r/ethereum/comments/4r0s2x/ringtoken_uploaded_into_testnet_but_project_will/ for details


# Ring Token 
 Ring Token consist of 3 parts, the contract, the frontend, and the cryptos. The contract is written in mixed Solidity/Assembly while the fronted used nwjs and the cryptos is written in C.
 
 Here is the rationale of the selection. The parts in Solidity is written for readability, the Assembly is for parts that requires optimization, nwjs is because of the most straightforward integration with the node while also has an acceptable UI. C is used for crypto simply because I am more familiar with libsecp256k1 which also happens to be written in C.
 
## How to Build
 
### The Contract
 
#### Solidity 
 
 I used the [realtime compiler] to compile and used the web3 deployment code while substituting the appropriate addresses for library used.
 
#### Assembly
 
 I used my own ethasm to generate the bytecode, manually add the init part and send the transaction with the bytecode as data while omitting the to. I think it would be more expensive to do this after Homestead but I'm not yet familiar with the alternatives.
 
### The Frontend 
 
 The frontend uses nwjs, which is just Node.js with the added DOM. 
 
 Currently it depends on the following Node.js modules:
 
 * [Level] You can install this with `npm install level`. Be warned though, to get it to work with nwjs you will need to redo leveldown.node file using [nw-gyp] and not node-gyp and replace the file in \node_modules\level\node_modules\leveldown\build\Release in Windows, not sure how it works in Linux (OK I'm going to be honest with you, my memory is a little bit fuzzy on how I made it work). Level is required for saving Pubkeys/Privkeys as a database.
 * [BigNumber.js] for BigNumber operation, I think Web3 already has it included but for some reason I nwjs can't find them. You can install this with `npm install bignumber.js`.
 * [Web3.js] Obviously to communicate with the Ethereum Node. You can install this with `npm install web3`.
 
Normally I did the testing by running `nw ./` on the folder containing the html file since I've already provided the packages.json inside but if a standalone package is required there's a [deployment instruction here].

### The Cryptos

I used some part of libsecp256k1 and sha3 from Ethereum's repo. It is included in the folder. Since everything is a header I just include them all. I used [nw-gyp] to create the .node file and copy that file to the root of the .html file.

# Usage Note

Folder pubkeys, privkeys, and keychain.txt need to be copied between Mint and Mix to keep it synchronized.

# Donation

Well, if you want to give me money I won't say no :) ETH(0x76fb135944db90c440fe881e07e7f5d7e6b82bcb)

 [realtime compiler]: https://chriseth.github.io/browser-solidity/
[Level]: https://github.com/Level
[nw-gyp]: https://github.com/nwjs/nw-gyp
[BigNumber.js]: https://github.com/MikeMcl/bignumber.js/
[Web3.js]: https://github.com/ethereum/web3.js
[deployment instruction here]: https://github.com/nwjs/nw.js/wiki/how-to-package-and-distribute-your-apps
