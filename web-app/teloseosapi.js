const { Api, JsonRpc, RpcError } = require('eosjs')
const { JsSignatureProvider } = require('eosjs/dist/eosjs-jssig')
const fetch = require('node-fetch')
// const { TextEncoder, TextDecoder } = require('util')
const { TextEncoder, TextDecoder } = require('text-encoding')

const signatureProvider = new JsSignatureProvider([
    '5JQT9qoFwHq6fi4HmG8x6kdXFKJNGyRN7bz8yRqdbJ3omiaDVKK',
    '5Khg26SQ9F2okyjtJQWW1kVd4jMQeZwTeaAEE4ppFyDVinUA2nr'
])

const rpc = new JsonRpc('http://testnet.theteloscope.io:18888', { fetch })
const api = new Api(
    {
        rpc,
        signatureProvider,
        textDecoder: new TextDecoder(),
        textEncoder: new TextEncoder()
    })

module.exports = {
    transact: (actor, action, data) => {

        return api.transact(
            {
                actions: [
                    {
                        account: 'eosyelosbobb',
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
    },

    info: async (account) => await rpc.get_account(account),

    getInventoryReqs: async () => await rpc.get_table_rows({
        json: true,                
        code: 'eosyelosbobb',         
        scope: 'eosyelosbobb',         
        table: 'inventreqs',   
        limit: 100,                  
        reverse: false,      
        show_payer: false,
    }),

    getCurrencyBal: async (account, symbol) => await rpc.get_currency_balance("eosyelosbobb", account, symbol)
}
