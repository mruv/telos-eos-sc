import ReactDOM from 'react-dom'
import React from 'react'


class TelosEos extends React.Component {

	constructor(props) { super(props) }

	render() {
		return <p>Hello Telos EOS </p>
	}
}


ReactDOM.render(<TelosEos/>, document.getElementById('app'))