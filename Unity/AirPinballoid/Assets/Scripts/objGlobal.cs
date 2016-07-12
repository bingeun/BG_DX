using UnityEngine;
using System.Collections;

public class objGlobal : MonoBehaviour
{
    void Start()
    {
    }

    void Update()
    {
        GameObject.FindWithTag("DirLight").transform.Rotate(0, Time.deltaTime * 50.0f, 0, Space.World);

        if (Application.platform == RuntimePlatform.Android)
        {
            if (Input.GetKey(KeyCode.Escape))
            {
                Application.Quit();
                return;
            }
        }
    }
}
