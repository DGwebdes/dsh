import { useState } from "react";
import Button from "./Button";
import Component from "./Component";

function App() {
    const [count, setCount] = useState(0);
    function handleClick() {
        setCount(count + 1);
    }
    return (
        <div>
            <h1>cPanel + Github workflow</h1>
            <p>A small update to test</p>
            <Button
                handleClick={handleClick}
                count={count}
            />
            <Component />
        </div>
    );
}

export default App;
