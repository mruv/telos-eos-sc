import React from 'react'
import { Container, Card, ListGroup, Row, Col, Button, Form, FormControl } from 'react-bootstrap';

export default class Erik extends React.Component {
    constructor(props) { super(props) }

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
                            {iron}
                        </ListGroup.Item>
                    </ListGroup>
                </Card.Body>
                <Card.Header>Inventory Requests</Card.Header>
                <Card.Body>
                    <Container>
                        <ListGroup>
                            <ListGroup.Item key={1000}>
                                <Row style={{ color: 'teal' }}>
                                    <Col>From</Col>
                                    <Col>Quantity <i>(lbs)</i></Col>
                                    <Col>Price</Col>
                                    <Col>Status</Col>
                                    <Col><span></span></Col>
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
                                                        req.status == "PENDING" ?
                                                            <Button
                                                                variant="outline-primary" size="sm"
                                                                onClick={(e) => {
                                                                    onPushAction({
                                                                        actor: name,
                                                                        action: "fulfill",
                                                                        data: {
                                                                            merchantAcct: name,
                                                                            irId: index
                                                                        }
                                                                    })
                                                                }
                                                                }>Fulfill</Button>
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