using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class finger1Seg1 : MonoBehaviour {
	public finger1Seg2 seg;

	// Use this for initialization
	void Start () {
		seg = FindObjectOfType(typeof(finger1Seg2)) as finger1Seg2;
	}
	
	// Update is called once per frame
	void Update () {
		
	}


//		float delay = 0.25f;
//		float factor = 1.0f / (1.0f - delay);
//		float scaledAmount = Mathf.Max(0.0f, (amount - delay) * factor);

	public void Bend (float x, Vector3 palm) {
		float scaledAmount = -0.001190476f + 1.300992f * x + 2.0625f * x * x - 6.527778f * x * x * x + 4.166667f * x * x * x * x;
	
		float first = scaledAmount * 88;

//		transform.rotation = Quaternion.Euler(new Vector3(palm.x + 90 + first, palm.y, 0));

		// x bends up/down
		// y bends into the screen / towards me
		// z bends into the screen / towards me
//		transform.rotation = Quaternion.Euler(new Vector3(palm.x + 90 + first, palm.y, 0));

		transform.rotation = Quaternion.Euler(new Vector3(palm.x + 90 + first, -90, 0));
		seg.Bend (first, x, palm);
	}
}
