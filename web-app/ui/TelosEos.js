import React, { Fragment } from 'react'
import NavBar from './NavBar';
import Main from './Main';
import { HashRouter } from 'react-router-dom';
import Axios from 'axios';

export default class TelosEos extends React.Component {
    constructor(props) {
        super(props)
    }

    componentDidMount() {
        Axios.get("/info").then((res) => {
            console.log(res)
        })
    }

    render() {
        return (
            <HashRouter>
                <Fragment>
                    <NavBar />
                    <Main />
                </Fragment>
            </HashRouter>
        )
    }
}