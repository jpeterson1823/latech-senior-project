<?php
    if ($_SERVER['REQUEST_METHOD'] == 'GET') {
        require 'connection.php';
        
        $sql = "SELECT * FROM Hardware";
        $result = $conn->query($sql);

        if ($result->num_rows > 0) {
            $return_array = array();
            $some_data = array();
            $more_data = array();
            while($row = $result->fetch_assoc()) {
                $other_data = array();
                $some_data[] = "<button class='btn' id='sensor". $row['idHardware'] . "'>" . $row['sensor_name'] . "</button>";
                if ($row['setting1'] != 'null'){
                    $other_data[] = 
                        "<div>
                            <p>Left Field of View:</p>
                            <div>
                                <input type='range' id='lfov' name='lfov' min='-45' max='45' value='"
                                . $row['setting1'] . "' oninput='this.nextElementSibling.value = this.value' />
                                <output>" . $row['setting1'] . "</output>
                                <label for='lfov'>LFOV</label>
                            </div>
                        </div>";
                }
                if ($row['setting2'] != 'null'){
                    $other_data[] = 
                        "<div>
                            <p>Right Field of View:</p>
                            <div>
                                <input type='range' id='rfov' name='rfov' min='-45' max='45' value='"
                                . $row['setting2'] . "' oninput='this.nextElementSibling.value = this.value' />
                                <output>" . $row['setting2'] . "</output>
                                <label for='rfov'>RFOV</label>
                            </div>
                        </div>";
                }
                if ($row['setting3'] != 'null'){
                    $other_data[] = 
                        "<div>
                            <p>Sensitivity:</p>
                            <div>
                                <input type='range' id='sense' name='sense' min='0' max='100' value='"
                                    . $row['setting3'] . "' oninput='this.nextElementSibling.value = this.value' />
                                <output>" . $row['setting3'] . "</output>
                                <label for='sense'></label>
                            </div>
                        </div>";
                }
                if (!empty($other_data)){
                    $other_data[] =
                        "<div>
                            <input type='submit' value='Save Changes'>
                            <input type='hidden' name='HardwareID' value='" . $row['idHardware'] . "'>
                        </div>";
                }
                $more_data[] = $other_data;
            }
            $return_array[] = $some_data;
            $return_array[] = $more_data;
            echo json_encode($return_array);
        } else {
            echo "";
        }
    }
?>