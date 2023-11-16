const express = require("express")
const app = express();

const port = 3000;

app.get("/",(req,res)=>{
    res.send("<h1>This is homepage</h1>");
})

app.get("/about",(req,res)=>{
    res.send("<h1>This is about page</h1>");
})

app.listen(port,()=>{
    console.log('server is working on http//:localhost:3000');
})
