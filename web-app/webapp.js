const express = require('express')
const teloseosapi = require('./teloseosapi')

const app = express()
const port = 3000

app.set('views', __dirname);
app.engine('html', require('ejs').renderFile);
app.set('view engine', 'ejs');
app.use(express.static('resources'))

app.get('/', (req, res) => res.render('index.html'))

app.get('/info', async (req, res) => {
    res.json(await teloseosapi.info())
})


app.listen(port, () => console.log(`Example app listening on port ${port}!`))