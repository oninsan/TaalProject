const express = require("express");
const cors = require("cors");

const {TaalReadings, TaalAccounts, db, addDoc, getDocs, query, orderBy, limit, where,startAt, endAt} = require("./config.js");
const { getDoc, doc } = require("firebase/firestore");
const app = express();
app.use(express.json());
app.use(cors());

const PORT = 5000;

// api
app.post("/api/taal/create", async(req, res)=>{
    try {
        const data = req.body;
        // console.log(data);
        await addDoc(TaalReadings, data);
        res.send({msg:"Added Taal readings"});
    } catch (error) {
        console.error(error);
        res.status(500).send({msg: "An error occurred"});
    }
})

app.post("/api/taal/readings", async (req, res) => {
  try {
      const { startDate, endDate } = req.body
      const q = query(
        TaalReadings,
        orderBy("DateTime"),
        where("DateTime", ">=", startDate),
        where("DateTime", "<=", endDate)
      )
      const querySnapshot = await getDocs(q);

      const docs = querySnapshot.docs.map(doc => doc.data())
      res.send({ msg: "Retrieved Taal readings", data: docs })
  } catch (error) {
      console.error(error);
      res.status(500).send({ msg: "An error occurred" })
  }
});

// get all accounts
app.post("/api/taal/accounts", async (req, res) => {
  try {
    const {username, password} = req.body
    const querySnapshot = await getDocs(query(TaalAccounts, where("Username", "==", username), where("Password","==",password)));
    const docs = querySnapshot.docs.map(doc => doc.data());
    docs.length>0?res.send({"AccFound": true}):res.send({"AccFound": false})
  } catch (error) {
    console.error(error);
    res.status(500).send({ msg: "An error occurred", error: error.message });
  }
});

app.post("/api/taal/createaccount", async (req, res) => {
  try {
    const data = req.body;
    if (!data || !data.Username) {
      res.status(400).send({ msg: "Invalid request data" });
      return;
    }

    const snapshot = await getDocs(query(TaalAccounts, where("Username", "==", data.Username)));
    if (!snapshot.empty) {
      res.status(200).send({ msg: "Username already exists" })
      return;
    }
    await addDoc(TaalAccounts, data);
    res.send({msg:"Account registered successfully"})
  } catch (error) {
    console.error(error);
    res.status(500).send({ msg: "An error occurred", error: error.message });
  }
})



// app.post("/api/taal/create", async(req, res)=>{
//     try {
//         const data = req.body;
//         console.log(data);

//         // Get the last index used
//         const lastDocSnapshot = await getDocs(query(TaalReadings, orderBy('Index', 'desc'), limit(1)));
//         const lastIndex = lastDocSnapshot.docs[0].data().Index;

//         // Add the index to the data
//         data.Index = lastIndex + 1;

//         // Add the document
//         await addDoc(TaalReadings, data);

//         res.send({msg:"Added Taal readings"});
//     } catch (error) {
//         console.error(error);
//         res.status(500).send({msg: "An error occurred"});
//     }
// });

// getting turbidity
app.get("/api/taal/getreadings/turbidity", async(req, res)=>{
    const ref = db.ref("/readings/data/turbidity"); // replace with your path

    // res.send(ref)
  ref.once("value", (snapshot) => {
    const data = snapshot.val();
    res.send(data);
  }, (error) => {
    console.error(error);
    res.status(500).send({msg: "An error occurred"});
  });
});

// getting Acidity
app.get("/api/taal/getreadings/ph", async(req, res)=>{
  const ref = db.ref("/readings/data/ph"); // replace with your path

  // res.send(ref)
ref.once("value", (snapshot) => {
  const data = snapshot.val();
  res.send(data);
  },(error) => {
    console.error(error);
    res.status(500).send({msg: "An error occurred"});
  });
});

// gettin temperature
app.get("/api/taal/getreadings/temperature", async(req, res)=>{
  const ref = db.ref("/readings/data/temperature"); // replace with your path

  // res.send(ref)
ref.once("value", (snapshot) => {
  const data = snapshot.val();
  res.send(data);
  },(error) => {
    console.error(error);
    res.status(500).send({msg: "An error occurred"});
  });
});


app.listen(PORT, ()=>console.log(`Server running on port ${PORT}`));
