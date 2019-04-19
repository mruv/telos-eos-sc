import React from 'react'
import { Container, Card, ListGroup, Row, Col, Button, Form, FormControl } from 'react-bootstrap';

export default class Bob extends React.Component {
    constructor(props) {
        super(props)
        const { iron } = this.props
        this.state = {
            availIron: parseFloat(iron.split(' ')[0]),
            sellIron: 0.0
        }

        this.handleChange = this.handleChange.bind(this)
    }

    handleChange(e) {

        e.persist()
        const value = e.target.value
        this.setState((state) => { return {sellIron: value}})
    }

    render() {
        const { name, tlos, ylos, iron, invReqs, onPushAction } = this.props

        return (
            <Card style={{ marginTop: '12px' }}>
                <Card.Header>Account</Card.Header>
                <Card.Body>
                    <Row>
                        <Col>Owner</Col><Col style={{ color: 'teal', fontSize: "14px" }}>{name}</Col>
                    </Row>
                </Card.Body>
                <Card.Header>Currencies</Card.Header>
                <Card.Body>
                    <ListGroup>
                        <ListGroup.Item>{tlos}</ListGroup.Item>
                        <ListGroup.Item>{ylos}</ListGroup.Item>
                    </ListGroup>
                </Card.Body>
                <Card.Header>Commodities</Card.Header>
                <Card.Body>
                    <ListGroup>
                        <ListGroup.Item>
                            <Row>
                                <Col>{iron}</Col>
                                <Col>
                                    <Form inline>
                                        <FormControl
                                            size="sm" value={this.state.sellIron} type="text"
                                            placeholder="Simulate Stock Sale"
                                            onChange={this.handleChange} />
                                        <Button 
                                        variant="outline-primary" size="sm"
                                        onClick={(e) => {
                                            e.persist()
                                            onPushAction({
                                                actor: name,
                                                action: "sell",
                                                data: {
                                                    payer: name,
                                                    quantity: parseFloat(this.state.sellIron).toFixed(1) + ' IRON'
                                                }
                                            })
                                        }}>Sell</Button>
                                    </Form>
                                </Col>
                            </Row>
                        </ListGroup.Item>
                    </ListGroup>
                </Card.Body>
                <Card.Header>Inventory Requests</Card.Header>
                <Card.Body>
                    <Container>
                        <ListGroup>
                            <ListGroup.Item key={1000}>
                                <Row style={{ color: 'teal' }}>
                                    <Col>To</Col>
                                    <Col>Quantity <i>(lbs)</i></Col>
                                    <Col>Price</Col>
                                    <Col>Status</Col>
                                    <Col>
                                        <Button variant="outline-primary" size="sm">New</Button>
                                    </Col>
                                </Row>
                            </ListGroup.Item>
                            {
                                invReqs.map((req, index) => {
                                    return (
                                        <ListGroup.Item key={index} size="sm">
                                            <Row>
                                                <Col>{req.to}</Col>
                                                <Col>{req.quantity}</Col>
                                                <Col>{req.unit_price} / <i>lbs</i></Col>
                                                <Col>{req.status}</Col>
                                                <Col>
                                                    {
                                                        req.status == "FULFILLED" ?
                                                            <Button
                                                                variant="outline-primary" size="sm"
                                                                onClick={(e) => {
                                                                    onPushAction({
                                                                        actor: name,
                                                                        action: "pay",
                                                                        data: {
                                                                            customerAcct: name,
                                                                            irId: index
                                                                        }
                                                                    })
                                                                }
                                                                }>Pay</Button>
                                                            : <span></span>
                                                    }
                                                </Col>
                                            </Row>
                                        </ListGroup.Item>
                                    )
                                })
                            }
                        </ListGroup>
                    </Container>
                </Card.Body>
            </Card>
        )
    }
}