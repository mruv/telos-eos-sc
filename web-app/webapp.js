const express = require('express')
const bodyParser = require('body-parser');
const teloseosapi = require('./teloseosapi')

const app = express()
const port = 3000

// config
app.set('views', __dirname);
app.engine('html', require('ejs').renderFile);
app.set('view engine', 'ejs');
app.use(express.static('resources'))
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));


// Routes
app.get('/', (req, res) => res.render('index.html'))

// get basic user info -- RAM, BANDWIDTH, TLOS balance, ...
app.get('/info', async (req, res) => {

    const bob = await teloseosapi.info('eosyelosbobb')
    const erik = await teloseosapi.info('eosyeloserik')

    res.json(
        {
            bob: {
                tlos: bob.core_liquid_balance,
                name: bob.account_name
            },
            erik: {
                tlos: erik.core_liquid_balance,
                name: erik.account_name
            }
        })
})

// get a list of Inventory Requests
app.get('/invreqs', async (req, res) => {
    res.json((await teloseosapi.getInventoryReqs()).rows)
})

// get currency balance for a token asset -- YELOS or IRON
app.get('/assets_bal', async (req, res) => {

    const bob_y = await teloseosapi.getCurrencyBal('eosyelosbobb', 'YELOS')
    const bob_i = await teloseosapi.getCurrencyBal('eosyelosbobb', 'IRON')
    const erik_y = await teloseosapi.getCurrencyBal('eosyeloserik', 'YELOS')
    const erik_i = await teloseosapi.getCurrencyBal('eosyeloserik', 'IRON')

    res.json({
        bob: { ylos: bob_y[0] || '', iron: bob_i[0] || '' },
        erik: { ylos: erik_y[0] || '', iron: erik_i[0] || '' }
    })
})

// push an action
app.post('/transact', async (req, res) => {
    // console.log(req.body)
    const {actor, action, data} = req.body
    const {code, resData} = await teloseosapi.transact(actor, action, data)
    res.status(code).json(resData)
})

// start web app
app.listen(port, () => console.log(`Example app listening on port ${port}!`))