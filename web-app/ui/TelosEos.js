import React, { Fragment } from 'react'
import NavBar from './NavBar';
import Main from './Main';
import { HashRouter } from 'react-router-dom';

export default class TelosEos extends React.Component {
    constructor(props) {
        super(props)
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