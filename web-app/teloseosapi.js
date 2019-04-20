const { Api, JsonRpc, RpcError } = require('eosjs')
const { JsSignatureProvider } = require('eosjs/dist/eosjs-jssig')
const fetch = require('node-fetch')
const { TextEncoder, TextDecoder } = require('text-encoding')

// Bob's  -- eosyelosbobb
// and
// Erik's -- eosyeloserik
// private keys
const signatureProvider = new JsSignatureProvider([
    '5JQT9qoFwHq6fi4HmG8x6kdXFKJNGyRN7bz8yRqdbJ3omiaDVKK',
    '5Khg26SQ9F2okyjtJQWW1kVd4jMQeZwTeaAEE4ppFyDVinUA2nr'
])

// Telos EOS RPC API 
const rpc = new JsonRpc('http://testnet.theteloscope.io:18888', { fetch })
const api = new Api(
    {
        rpc,
        signatureProvider,
        textDecoder: new TextDecoder(),
        textEncoder: new TextEncoder()
    })

module.exports = {

    // CLEOS PUSH ACTION CONTRACT_ACCT='eosyelosbobb' ACTION=action DATA=data PERMISSION='actor@active'
    // 
    transact: async (actor, action, data) => {

        try {
            const resData = await api.transact(
                {
                    actions: [
                        {
                            account: 'eosyelosbobb', // smart contract owner
                            name: action,
                            authorization: [
                                { actor: actor, permission: 'active', }
                            ],
                            data: data,
                        }]
                },
                {
                    blocksBehind: 3,
                    expireSeconds: 30,
                })
            return { code: 200, resData: resData }
        } catch (error) {
            return {code: 500}
        }
    },

    // CLEOS GET ACCOUNT NAME=account
    //
    info: (account) => rpc.get_account(account),

    // CLEOS GET TABLE ACCOUNT='eosyelosbobb' SCOPE='eosyelosbobb' TABLE='inventreqs' LIMIT=100
    //
    getInventoryReqs: () => rpc.get_table_rows({
        json: true,
        code: 'eosyelosbobb',
        scope: 'eosyelosbobb',
        table: 'inventreqs',
        limit: 100,
        reverse: false,
        show_payer: false,
    }),

    // CLEOS GET CURRENCY BALANCE CONTRACT_ACCT='eosyelosbobb' ACCOUNT=account SYMBOL=symbol
    //
    getCurrencyBal: (account, symbol) => rpc.get_currency_balance("eosyelosbobb", account, symbol)
}
