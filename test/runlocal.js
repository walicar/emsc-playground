const express = require('express');
const path = require('path');
const app = express();

var dir = path.join(__dirname, '..')

app.use('/assets', express.static(path.join(dir, 'web/assets')));

app.get('/', (req, res) => {
   res.sendFile(path.join(dir, 'web/index.html'));
});

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
   console.log(`Server is running on port http://localhost:${PORT}`);
});
