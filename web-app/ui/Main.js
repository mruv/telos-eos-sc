import React from 'react'
import { Tabs, Tab, Container, Spinner } from 'react-bootstrap';
import Bob from './Bob';
import Erik from './Erik';
import Axios from 'axios';

export default class Main extends React.Component {
    constructor(props) {
        super(props)

        this.state = {
            isLoading: true, 
            bob: { name: '', tlos: '', ylos: '', iron: '' },
            erik: { name: '', tlos: '', ylos: '', iron: '' }
        }

        this.fetchInvReqs = this.fetchInvReqs.bind(this)
        this.fetchYelosBal = this.fetchYelosBal.bind(this)
    }



    componentDidMount() {
        Axios.get("/info").then((res) => {
            this.setState((state) => {
                const bob_e = state.bob
                const erik_e = state.erik
                const { bob, erik } = res.data

                bob_e.tlos = bob.tlos
                bob_e.name = bob.name
                erik_e.tlos = erik.tlos
                erik_e.name = erik.name

                return {
                    bob: res.data.bob,
                    erik: res.data.erik
                }
            })
        })

        this.fetchInvReqs()
        this.fetchYelosBal()
    }

    fetchInvReqs() {
        Axios.get("/invreqs").then((res) => {
            this.setState((state) => {
                return { invReqs: res.data }
            })

            console.log(this.state)
        })
    }

    fetchYelosBal() {
        Axios.get("/assets_bal").then((res) => {
            this.setState((state) => {
                const bob_e = state.bob
                const erik_e = state.erik
                const { bob, erik } = res.data

                bob_e.ylos = bob.ylos
                bob_e.iron = bob.iron
                erik_e.ylos = erik.ylos
                erik_e.iron = erik.iron

                return {
                    bob: bob_e,
                    erik: erik_e,
                    isLoading: false
                }
            })

            console.log(this.state)
        })
    }

    fetchCommPrices() {
        Axios.get("/curr_bal").then((res) => {
            this.setState((state) => {
                return { ...res.data }
            })

            console.log(this.state)
        })
    }

    render() {
        const { bob, erik, isLoading, invReqs } = this.state

        const view = isLoading ?
            <Spinner animation="grow" /> : (
                <Container style={{ marginTop: '20px' }}>
                    <Tabs defaultActiveKey="bob">
                        <Tab eventKey="bob" title="Bob">
                            <Bob {...bob} invReqs={invReqs} />
                        </Tab>
                        <Tab eventKey="erik" title="Erik">
                            <Erik {...erik} invReqs={invReqs} />
                        </Tab>
                    </Tabs>
                </Container>
            )

        return view
    }
}