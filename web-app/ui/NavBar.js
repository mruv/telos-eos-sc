import React from 'react'
import { Navbar } from 'react-bootstrap'

export default class NavBar extends React.Component {
  constructor(props) { super(props) }

  render() {
    return (
      <Navbar bg="light" expand="lg">
        <Navbar.Brand>Scrap Metal Business</Navbar.Brand>
      </Navbar>
    )
  }
}