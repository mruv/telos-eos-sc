import React, { Fragment } from 'react'
import { Card, Container, ListGroup, Col, Row } from 'react-bootstrap';

export default class Erik extends React.Component {
    constructor(props) { super(props) }

    render() {
        const { name, tlos, ylos, iron, invReqs } = this.props

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
                        <ListGroup.Item>{iron}</ListGroup.Item>
                    </ListGroup>
                </Card.Body>
                <Card.Header>Inventory Requests</Card.Header>
                <Card.Body>
                    <Container>
                        <ListGroup>
                            <ListGroup.Item key={1000}>
                                <Row style={{color: 'teal'}}>
                                <Col>From</Col><Col>Quantity</Col><Col>Price</Col><Col>Status</Col>
                                </Row>
                            </ListGroup.Item>
                            {
                                invReqs.map((req, index) => {
                                    return (
                                        <ListGroup.Item key={index}>
                                            <Row>
                                                <Col>{req.from}</Col>
                                                <Col>{req.quantity}</Col>
                                                <Col>{req.unit_price} / <i>lbs</i></Col>
                                                <Col>{req.status}</Col>
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