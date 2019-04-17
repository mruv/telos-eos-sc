var path = require('path');

const webpack = require('webpack');

module.exports = {
    entry: './ui/index.js',
    devtool: 'sourcemaps',
    cache: false,
    mode: 'development',
    output: {
        path: __dirname,
        filename: './resources/built/bundle.js'
    }, module: {
        rules: [{
                test: path.join(__dirname, '.'),
                exclude: /(node_modules)/,
                use: [{
                        loader: 'babel-loader',
                        options: {
                            presets: ["@babel/preset-env", "@babel/preset-react"]
                        }
                    }]
            },
            {
                test: /\.css$/,
                use: [{loader: 'style-loader'}, {loader: 'css-loader'}]
            },
            {
                test: /\.(ttf|eot|svg|woff|woff2)(\?v=[0-9]\.[0-9]\.[0-9])?$/,
                use: [{loader: 'file-loader'}]
            },
            {
                test: /\.(png|jpg|gif)$/,
                use: [{loader: 'file-loader'}]
            },
            {
                test: /\.(ttf|eot|woff|woff2|svg)$/,
                loader: "url-loader?limit=10&name=fonts/[hash].[ext]"
            }
        ]
    }, plugins: [new webpack.ProvidePlugin({"React": "react"})]
};

